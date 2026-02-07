#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/utils/async.hpp"
#include "Geode/utils/file.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Popup.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

class ImportPopup : public geode::Popup, TextInputDelegate {
protected:
    int m_zOrder = 0;
    int m_objsCount = 0;
    float m_drawScale = 1;
    matjson::Value m_jsonSets;
    GameObject* m_centerObj = nullptr;
    TextInput* m_zLayerInput = nullptr;
    CCLabelBMFont* m_fileLabel = nullptr;
    CCLabelBMFont* m_countLabel = nullptr;
    TextInput* m_drawScaleInput = nullptr;
    CCMenuItemSpriteExtra* m_selectBtn = nullptr;
    CCMenuItemSpriteExtra* m_changeBtn = nullptr;
    static constexpr CCSize m_popupSize = CCSize(275.f, 245.f);
    async::TaskHolder<file::PickResult> m_pickHolder;
protected:
    void parseAndPlace();
    void importJSON(cocos2d::CCObject* sender);
    void checkAlert(cocos2d::CCObject* sender);
    void onHelp(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode *p0) override;
    bool init(cocos2d::CCArray* selectedObj);
public:
    static ImportPopup* create(cocos2d::CCArray* selectedObj);
};
