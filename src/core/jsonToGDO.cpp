#include "./jsonToGDO.h"

#include <Geode/utils/async.hpp>

core::json2gdo::ParseResult core::json2gdo::parse(const matjson::Value &json, ParseOptions options) {
    std::ostringstream objsString;
    int objsCount = 0;

    for (auto obj : json) {
        // Avoiding objects with zero score
        auto objScore = obj["score"].asDouble();
        if (!objScore || objScore.unwrap() <= 0) {
            continue;
        }

        // Setting and initializing default properties if there is missing some
        float posX = options.centerObj->getPositionX();
        float posY = options.centerObj->getPositionY();
        float scaleX = 1.f, scaleY = 1.f, rotation = 0.f;
        auto redResult = obj["color"][0].asDouble();
        auto blueResult = obj["color"][1].asDouble();
        auto greenResult = obj["color"][2].asDouble();

        // Validating types
        if (auto objType = obj["type"].asInt()) {
            if (!m_validObjTypes.contains(objType.unwrap()))
                continue;
            else
                objsCount++;
        }

        if (auto posXResult = obj["data"][0].asDouble()) {
            posX += posXResult.unwrap() * options.drawScale;
        }
        if (auto posYResult = obj["data"][1].asDouble()) {
            posY += posYResult.unwrap() * options.drawScale;
        }
        if (auto scaleXResult = obj["data"][2].asDouble()) {
            scaleX = scaleXResult.unwrap() * options.drawScale / 4 * 0.16;
        }
        if (auto scaleYResult = obj["data"][3].asDouble()) {
            scaleY = scaleYResult.unwrap() * options.drawScale / 4 * 0.16;
        } else {
            scaleY = scaleX;
        }
        if (auto rotationResult = obj["data"][4].asDouble()) {
            rotation = -rotationResult.unwrap();
        }

        // Parsing colors
        float h = 0.f, s = 0.f, v = 0.f;
        if (redResult && blueResult && greenResult) {
            rgbToHsv(
                redResult.unwrap() / 255.f,
                blueResult.unwrap() / 255.f,
                greenResult.unwrap() / 255.f,
                h,s,v
            );
        }

        // Parsing objects to gd format and increasing Z Order
        objsString << fmt::format(
            "1,{},2,{},3,{},128,{},129,{},6,{},41,1,42,1,21,1010,22,1010,43,{}a{}a{}a1a1,44,{}a{}a{}a1a1,25,{},372,1;",
            m_circleID, posX, posY, scaleX, scaleY, rotation,
            h,s,v, h,s,v, options.zOrderOffset
        );
        options.zOrderOffset++;
    }

    return ParseResult {.objects = objsString.str(), .objectsCount = objsCount};
}

arc::Future<core::json2gdo::ParseResult> core::json2gdo::asyncParse(const matjson::Value &json, ParseOptions options) {
    auto handle = geode::async::runtime().spawnBlocking<ParseResult>(
        [json, options]() {
            return parse(json, options);
        }
    );

    auto result = co_await handle;
    co_return result;
}

void core::json2gdo::rgbToHsv(float fR, float fG, float fB, float &fH, float &fS, float &fV) {
    // This function is took from https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72#file-hsvrgb-cpp-L53
    float fCMax = std::max(std::max(fR, fG), fB);
    float fCMin = std::min(std::min(fR, fG), fB);
    float fDelta = fCMax - fCMin;

    if(fDelta > 0) {
        if(fCMax == fR) {
            fH = 60 * (fmod(((fG - fB) / fDelta), 6));
        } else if(fCMax == fG) {
            fH = 60 * (((fB - fR) / fDelta) + 2);
        } else if(fCMax == fB) {
            fH = 60 * (((fR - fG) / fDelta) + 4);
        }

        if (fCMax > 0) {
            fS = fDelta / fCMax;
        } else {
            fS = 0;
        }

        fV = fCMax;
    } else {
        fH = 0;
        fS = 0;
        fV = fCMax;
    }

    if(fH < 0) {
        fH = 360 + fH;
    }
}
