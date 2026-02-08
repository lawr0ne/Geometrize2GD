#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/utils/async.hpp"
#include "Geode/utils/file.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Popup.hpp>
#include <matjson.hpp>
#include "../core/jsonToGDO.h"

using namespace geode::prelude;

class ImportPopup : public geode::Popup, TextInputDelegate {
protected:
    bool m_allowedExit = true;
    int m_zOrderOffset = 0;
    int m_objsCount = 0;
    float m_drawScale = 1;
    matjson::Value m_jsonSets;
    std::string m_objsString;
    GameObject* m_centerObj = nullptr;
    TextInput* m_zLayerInput = nullptr;
    CCLabelBMFont* m_parsingText = nullptr;
    CCNode* m_parsedView = nullptr;
    CCMenu* m_parsedViewMenu = nullptr;
    CCLabelBMFont* m_fileLabel = nullptr;
    CCLabelBMFont* m_countLabel = nullptr;
    TextInput* m_drawScaleInput = nullptr;
    CCMenuItemSpriteExtra* m_selectBtn = nullptr;
    static constexpr CCSize m_popupSize = CCSize(275.f, 245.f);
    async::TaskHolder<file::PickResult> m_pickHolder;
    async::TaskHolder<core::json2gdo::ParseResult> m_parseHolder;
protected:
    bool init(cocos2d::CCArray* selectedObj);
    void onHelp(cocos2d::CCObject* sender);
    void importJSON(cocos2d::CCObject* sender);
    void onFilePicked(file::PickResult result);
    void onJSONParsed(core::json2gdo::ParseResult result);
    void checkAlert(cocos2d::CCObject* sender);
    void parseTextAnimationStep(CCNode* node);
    void place();
    void textChanged(CCTextInputNode *p0) override;
    void keyBackClicked() override;
public:
    static ImportPopup* create(cocos2d::CCArray* selectedObj);
};
