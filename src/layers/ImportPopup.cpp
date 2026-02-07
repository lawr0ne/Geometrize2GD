#include "ImportPopup.h"

#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/ui/Popup.hpp"
#include "Geode/utils/file.hpp"
#include <arc/future/Future.hpp>
#include "../types/ScopeExit.h"
#include "Geode/utils/web.hpp"
#include "../core/jsonToGDO.h"

ImportPopup* ImportPopup::create(CCArray* selectedObj) {
    ImportPopup* ret = new ImportPopup();
    if (ret && ret->init(selectedObj)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

// Setups the layer
bool ImportPopup::init(CCArray* selectedObj) {
    if (!Popup::init(this->m_popupSize.width, this->m_popupSize.height)) return false;

    this->m_centerObj = CCArrayExt<GameObject*>(selectedObj)[0];

    this->m_countLabel = CCLabelBMFont::create("Objects: 0", "bigFont.fnt");
    this->m_countLabel->setPosition({this->m_popupSize.width / 2, this->m_popupSize.height / 2 - 65.f});
    this->m_countLabel->setVisible(false);
    this->m_countLabel->setID("count-label");
    this->m_countLabel->setScale(0.4);

    auto drawLabel = CCLabelBMFont::create("Scale:", "bigFont.fnt");
    drawLabel->setPosition({this->m_popupSize.width / 2 + 65.f, this->m_popupSize.height / 2 + 15.f});
    drawLabel->setVisible(false);
    drawLabel->setID("draw-scale-label");
    drawLabel->setScale(0.5);

    auto zLayerLabel = CCLabelBMFont::create("Z-Layer\nOffset:", "bigFont.fnt");
    zLayerLabel->setPosition({this->m_popupSize.width / 2 - 65.f, this->m_popupSize.height / 2 + 15.f});
    zLayerLabel->setVisible(false);
    zLayerLabel->setID("zlayer-label");
    zLayerLabel->setScale(0.325);

    this->m_fileLabel = CCLabelBMFont::create("", "bigFont.fnt");
    this->m_fileLabel->setColor({0,255,0});
    this->m_fileLabel->setPosition(this->m_popupSize.width / 2, this->m_popupSize.height / 2 + 60.f);
    this->m_fileLabel->setScale(0.45);
    this->m_fileLabel->setID("file-label");
    this->m_fileLabel->setVisible(false);

    auto importJsonSpr = ButtonSprite::create("Select File");
    this->m_selectBtn = CCMenuItemSpriteExtra::create(
        importJsonSpr, this, menu_selector(ImportPopup::importJSON)
    );
    this->m_selectBtn->setID("import-btn");
    this->m_selectBtn->setPosition(this->m_selectBtn->getPosition() + this->m_popupSize / 2);

    auto changeJsonSpr = ButtonSprite::create("Change File");
    this->m_changeBtn = CCMenuItemSpriteExtra::create(
        changeJsonSpr, this, menu_selector(ImportPopup::importJSON)
    );
    this->m_changeBtn->setVisible(false);
    this->m_changeBtn->setPosition({this->m_popupSize.width / 2, this->m_popupSize.height / 2 + 90.f});
    this->m_changeBtn->setID("change-btn");

    auto parseSpr =  ButtonSprite::create("Create");
    auto parseBtn = CCMenuItemSpriteExtra::create(
        parseSpr, this, menu_selector(ImportPopup::checkAlert)
    );
    parseBtn->setPosition({ImportPopup::m_popupSize.width / 2, ImportPopup::m_popupSize.height / 2 - 95.f});
    parseBtn->setVisible(false);
    parseBtn->setID("convert-btn");

    auto helpSpr = CCSprite::createWithSpriteFrameName("GJ_helpBtn_001.png");
    auto helpBtn = CCMenuItemSpriteExtra::create(
        helpSpr, this, menu_selector(ImportPopup::onHelp)
    );
    helpBtn->setPosition(this->m_popupSize);

    this->m_drawScaleInput = TextInput::create(50.f, "Float", "bigFont.fnt");
    this->m_drawScaleInput->setCommonFilter(CommonFilter::Float);
    this->m_drawScaleInput->setMaxCharCount(5);
    this->m_drawScaleInput->setString("1");
    this->m_drawScaleInput->setID("draw-input");
    this->m_drawScaleInput->setPosition({this->m_popupSize.width / 2 + 65.f, this->m_popupSize.height / 2 - 15.f});
    this->m_drawScaleInput->setVisible(false);
    this->m_drawScaleInput->getInputNode()->setLabelPlaceholderScale(0.5);
    this->m_drawScaleInput->getInputNode()->setMaxLabelScale(0.6);
    this->m_drawScaleInput->setDelegate(static_cast<TextInputDelegate*>(this));

    this->m_zLayerInput = TextInput::create(50.f, "Int", "bigFont.fnt");
    this->m_zLayerInput->setCommonFilter(CommonFilter::Int);
    this->m_zLayerInput->setMaxCharCount(5);
    this->m_zLayerInput->setString("0");
    this->m_zLayerInput->setID("zlayer-input");
    this->m_zLayerInput->setPosition({this->m_popupSize.width / 2 - 65.f, this->m_popupSize.height / 2 - 15.f});
    this->m_zLayerInput->setVisible(false);
    this->m_zLayerInput->getInputNode()->setLabelPlaceholderScale(0.6);
    this->m_zLayerInput->getInputNode()->setMaxLabelScale(0.6);
    this->m_zLayerInput->setDelegate(static_cast<TextInputDelegate*>(this));

    this->m_mainLayer->addChild(this->m_fileLabel);
    this->m_mainLayer->addChild(this->m_countLabel);
    this->m_mainLayer->addChild(drawLabel);
    this->m_mainLayer->addChild(zLayerLabel);

    this->m_buttonMenu->addChild(this->m_selectBtn);
    this->m_buttonMenu->addChild(this->m_changeBtn);
    this->m_buttonMenu->addChild(parseBtn);
    this->m_buttonMenu->addChild(this->m_drawScaleInput);
    this->m_buttonMenu->addChild(this->m_zLayerInput);
    this->m_buttonMenu->addChild(helpBtn);
    return true;
}

void ImportPopup::onHelp(CCObject* sender) {
    geode::createQuickPopup(
        "Help", "Do you want to open the guide?",
        "Yes", "No",
        [](auto* self, bool btn2) {
            if (!btn2)
                geode::utils::web::openLinkInBrowser("https://github.com/lawr0ne/Geometrize2GD/blob/main/GUIDE.md");
        }
    );
}

void ImportPopup::importJSON(CCObject* sender) {
    // Setting file pick options
    file::FilePickOptions::Filter json_file = {
        .description = "Geometrize JSON Output",
        .files = { "*.json"}
    };
    file::FilePickOptions options = {
        std::nullopt,
        {json_file}
    };
    this->m_buttonMenu->setEnabled(false);

    this->m_pickHolder.spawn(
        file::pick(file::PickMode::OpenFile, options),
        [this](file::PickResult result) {
            auto enableBtns = ScopeExit([this]() {
                this->m_buttonMenu->setEnabled(true);
            });

            // Checks does Result is empty or not
            if (result.isErr()) {
                return Notification::create(
                    fmt::format("Failed to open the file. Error: {}", result.err()),
                    NotificationIcon::Error
                )->show();
            }

            auto path = result.unwrap();
            if (!path.has_value()) {
                return;
            }

            if (path->string().ends_with(".json")) {
                // Loading json
                auto json = geode::utils::file::readJson(*path);
                if (json) {
                    this->m_jsonSets = json.unwrap();
                }

                if (auto temp = this->m_jsonSets["shapes"].asArray())
                    this->m_jsonSets = temp.unwrap();
                else if (auto temp = this->m_jsonSets.asArray())
                    this->m_jsonSets = temp.unwrap();
                else {
                    return Notification::create(
                        "Failed to parse the file! It may not follows the guide.",
                        NotificationIcon::Error
                    )->show();
                }

                // Counts the objects
                for (auto obj : this->m_jsonSets) {
                    auto objType = obj["type"].asInt();
                    auto objScore = obj["score"].asDouble();

                    if (!objType)
                        continue;

                    if (!objScore)
                        continue;

                    if (!core::json2gdo::m_validObjTypes.contains(objType.unwrap()))
                        continue;

                    if (objScore.unwrap() <= 0)
                        continue;

                    this->m_objsCount++;
                }

                auto countText = fmt::format("Objects: {}", this->m_objsCount);
                auto fileText = fmt::format("File: {}", path->filename());
                this->m_countLabel->setString(countText.c_str());
                this->m_fileLabel->setString(fileText.c_str());
                this->m_fileLabel->limitLabelWidth(this->m_popupSize.width - 10, 0.45f, 0.2f);
                this->m_selectBtn->setVisible(false);
                this->m_buttonMenu->getChildByID("change-btn")->setVisible(true);
                this->m_mainLayer->getChildByID("draw-scale-label")->setVisible(true);
                this->m_mainLayer->getChildByID("zlayer-label")->setVisible(true);
                this->m_buttonMenu->getChildByID("convert-btn")->setVisible(true);
                this->m_fileLabel->setVisible(true);
                this->m_countLabel->setVisible(true);
                this->m_zLayerInput->setVisible(true);
                this->m_drawScaleInput->setVisible(true);
                this->m_objsCount = 0;

                Notification::create(
                    "File is imported",
                    NotificationIcon::Success
                )->show();
            } else {
                Notification::create(
                    "Wrong file format. It must be a .json file!",
                    NotificationIcon::Error
                )->show();
            }
        }
    );
}

// Parses the objects from Geometrize to GD format and places the objects inside GD Editor
void ImportPopup::parseAndPlace() {
    auto objsString = core::json2gdo::parse(this->m_jsonSets, this->m_centerObj, this->m_drawScale, this->m_zOrder);

    // Checking does it has parsed any objects
    if (objsString.empty()) {
        Notification::create(
            "No objects added.",
            NotificationIcon::Error
        )->show();
        return this->onClose(nullptr);
    }

    // Getting both LevelEditorLayer and EditorUI
    auto activeEditorLayer = LevelEditorLayer::get();
    auto activeEditorUI = activeEditorLayer->m_editorUI;

    // Deleting selected objects
    activeEditorUI->onDeleteSelected(nullptr);

    // Create objects from string and flip Y-axis
    auto objectsArray = activeEditorLayer->createObjectsFromString(objsString.c_str(), true, true);
    activeEditorUI->flipObjectsY(objectsArray);

    // Add to undo stack and select objects
    activeEditorLayer->m_undoObjects->addObject(UndoObject::createWithArray(objectsArray, UndoCommand::Paste));
    activeEditorUI->selectObjects(objectsArray, true);

    // Update UI and notify user
    activeEditorUI->updateButtons();
    Notification::create(
        "Successfully converted to gd objects!",
        NotificationIcon::Success
    )->show();

    // Closing the popup
    this->keyBackClicked();
}

// Checks does object count is bigger than 5k. If so, it shows a warning
void ImportPopup::checkAlert(CCObject* sender) {
    if (this->m_objsCount > 5000) {
        geode::createQuickPopup(
            "Alert",
            "This will place more than <cy>5000 objects</c>\nAre you sure?",
            "No", "Yes",
            [this](auto, bool btn2) {
                if (btn2) {
                    this->parseAndPlace();
                }
            }
        );
    } else {
        this->parseAndPlace();
    }
}

// Checks the value inside inputs to avoid unwanted crashes
void ImportPopup::textChanged(CCTextInputNode *p0) {
    if (p0 == this->m_drawScaleInput->getInputNode()) {
        auto num = utils::numFromString<float>(p0->getString());
        if (num.isErr()) {
            return p0->setLabelNormalColor(ccc3(255,0,0));
        }
        auto numUn = num.unwrap();
        if (numUn <= 0) {
            return p0->setLabelNormalColor(ccc3(255,0,0));
        }
        p0->setLabelNormalColor(ccc3(255,255,255));
        this->m_drawScale = numUn;
    } else if (p0 == this->m_zLayerInput->getInputNode()) {
        auto num = utils::numFromString<int>(p0->getString());
        if (num.isErr()) {
            return p0->setLabelNormalColor(ccc3(255,0,0));
        }
        auto numUn = num.unwrap();
        p0->setLabelNormalColor(ccc3(255,255,255));
        this->m_zOrder = numUn;
    }
}
