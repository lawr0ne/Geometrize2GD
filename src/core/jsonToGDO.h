#ifndef COREPARSE_H
#define COREPARSE_H

#include <matjson.hpp>
namespace core::json2gdo {
    namespace {
        constexpr int m_circleID = 497;
    }
    const std::set<int> m_validObjTypes = {5, 3, 4, 32, 8, 16};

    std::string parse(matjson::Value &json, GameObject *centerObj, float drawScale, int zOrderOffset);
    void rgbToHsv(float fR, float fG, float fB, float &fH, float &fS, float &fV);
}

#endif
