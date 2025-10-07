#include "Utils.hpp"
#include "Cache.hpp"

#include "../Hooks/SimplePlayer.hpp"

SimplePlayer* Utils::createIcon(IconType type, bool secondPlayer) {
    SimplePlayer* icon = SimplePlayer::create(1);

    icon->updatePlayerFrame(getIconID(type, secondPlayer), type);
    icon->disableGlowOutline();

    return icon;
}

CCMenuItemToggler* Utils::createTypeToggle(bool radial, cocos2d::CCPoint pos, CCObject* target, cocos2d::SEL_MenuHandler callback) {
    CCSprite* spr = CCSprite::create("GJ_button_04.png");

    CCSprite* spr2 = CCSprite::createWithSpriteFrameName(radial ? "edit_areaModeBtn04_001.png" : "edit_areaModeBtn03_001.png");
    spr2->setScale(1.6f);
    spr2->setPosition(spr->getContentSize() / 2.f);

    spr->addChild(spr2);

    CCSprite* spr3 = CCSprite::create("GJ_button_02.png");

    CCSprite* spr4 = CCSprite::createWithSpriteFrameName(radial ? "edit_areaModeBtn04_001.png" : "edit_areaModeBtn03_001.png");
    spr4->setScale(1.6f);
    spr4->setPosition(spr3->getContentSize() / 2.f);

    spr3->addChild(spr4);

    CCMenuItemToggler* toggle = CCMenuItemToggler::create(spr, spr3, target, callback);
    toggle->setScale(0.475f);
    toggle->setPosition(pos);

    return toggle;
}

int Utils::getIconID(IconType type, bool secondPlayer) {
    GameManager* gm = GameManager::get();

    if (!(secondPlayer && Loader::get()->isModLoaded("weebify.separate_dual_icons")))
        switch (type) {
            case IconType::Cube: return gm->getPlayerFrame();
            case IconType::Ship: return gm->getPlayerShip();
            case IconType::Ball: return gm->getPlayerBall();
            case IconType::Ufo: return gm->getPlayerBird();
            case IconType::Wave: return gm->getPlayerDart();
            case IconType::Robot: return gm->getPlayerRobot();
            case IconType::Spider: return gm->getPlayerSpider();
            case IconType::Swing: return gm->getPlayerSwing();
            case IconType::Jetpack: return gm->getPlayerJetpack();
            default: return gm->getPlayerFrame();
        }
    else {
        Mod* sdiMod =  Loader::get()->getLoadedMod("weebify.separate_dual_icons");
        switch (type) {
            case IconType::Cube: return sdiMod->getSavedValue<int>("cube", 1);
            case IconType::Ship: return sdiMod->getSavedValue<int>("ship", 1);
            case IconType::Ball: return sdiMod->getSavedValue<int>("roll", 1);
            case IconType::Ufo: return sdiMod->getSavedValue<int>("bird", 1);
            case IconType::Wave: return sdiMod->getSavedValue<int>("dart", 1);
            case IconType::Robot: return sdiMod->getSavedValue<int>("robot", 1);
            case IconType::Spider: return sdiMod->getSavedValue<int>("spider", 1);
            case IconType::Swing: return sdiMod->getSavedValue<int>("swing", 1);
            case IconType::Jetpack: return sdiMod->getSavedValue<int>("jetpack", 1);
            default: return sdiMod->getSavedValue<int>("cube", 1);
        }
    }
}

bool Utils::isGradientSaved(GradientConfig config) {
    for (const matjson::Value& obj : Mod::get()->getSavedValue<matjson::Value>("saved-gradients"))
        if (configFromObject(obj) == config)
            return true;

    return false;
}

bool Utils::isSettingEnabled(int setting) {
    switch (setting) {
        case MOD_DISABLED: return Cache::isModDisabled();
        case P2_DISABLED: return Cache::is2PDisabled();
        case P2_FLIP: return Cache::is2PFlip();
        case MENU_GRADIENTS: return Cache::isMenuGradientsEnabled();
        case P2_SEPARATE: return Cache::is2PSeparate();
    }

    return false;
}

GradientConfig Utils::getDefaultConfig(ColorType colorType) {
    GameManager* gm = GameManager::get();
    int color;
    switch (colorType) {
        case ColorType::Main:
            color = gm->getPlayerColor();
            break;
        case ColorType::Secondary:
            color = gm->getPlayerColor2();
            break;
        case ColorType::Glow:
            color = gm->getPlayerGlowColor();
            break;
        default:
            color = gm->getPlayerColor();
            break;
    }

    return {
        {
            {{0.5f, 1.1f}, gm->colorForIdx(color)},
            {{0.5f, -0.1f}, gm->colorForIdx(color)}
        },
        true
    };
}

matjson::Value Utils::getSaveObject(GradientConfig config) {
    matjson::Value ret = matjson::Value{};
    matjson::Value pointsObject = matjson::Value::array();

    for (SimplePoint point : config.points) {
        matjson::Value object = matjson::Value{};

        object["pos"]["x"] = point.pos.x;
        object["pos"]["y"] = point.pos.y;

        object["color"]["r"] = point.color.r;
        object["color"]["g"] = point.color.g;
        object["color"]["b"] = point.color.b;

        pointsObject.push(object);
    }

    ret["points"] = pointsObject;
    ret["linear"] = config.isLinear;

    return ret;
}

void Utils::removeSavedGradient(GradientConfig config) {
    matjson::Value newArray = matjson::Value::array();

    for (const matjson::Value& obj : Mod::get()->getSavedValue<matjson::Value>("saved-gradients"))
        if (configFromObject(obj) != config)
            newArray.push(obj);
    
    Mod::get()->setSavedValue("saved-gradients", newArray);
}

void Utils::saveConfig(GradientConfig config, const std::string& id, const std::string& secondId) {
    matjson::Value container = Mod::get()->getSavedValue<matjson::Value>(id);

    if (secondId.empty()) {
        if (!container.isArray()) 
            container = matjson::Value::array();

        container.push(getSaveObject(config));
    } else
        container[secondId] = getSaveObject(config);

    Mod::get()->setSavedValue(id, container);
}

GradientConfig Utils::configFromObject(const matjson::Value& object) {
    GradientConfig config;

    config.isLinear = object["linear"].asBool().unwrapOr(true);

    for (const matjson::Value& point : object["points"])
        config.points.push_back({
            ccp(
                point["pos"]["x"].asDouble().unwrapOr(0.0), 
                point["pos"]["y"].asDouble().unwrapOr(0.0)
            ),
            ccc3(
                point["color"]["r"].asInt().unwrapOr(0),
                point["color"]["g"].asInt().unwrapOr(0),
                point["color"]["b"].asInt().unwrapOr(0)
            )
        });

    return config;
}

GradientConfig Utils::getSavedConfig(IconType type, ColorType colorType, bool secondPlayer) {
    std::string id = getTypeID(type);
    std::string color = "color" + std::to_string(colorType);

    if (!Utils::isSettingEnabled(P2_SEPARATE))
        secondPlayer = false;

    if (secondPlayer)
        id += "-p2";

    GradientConfig config;

    if (!Mod::get()->hasSavedValue(id)) {
        std::string globalKey = secondPlayer ? "global-p2" : "global";

        if (!Mod::get()->hasSavedValue(globalKey)) {
            return getDefaultConfig(colorType);
        } else
            id = globalKey;
    }

    matjson::Value jsonConfig = Mod::get()->getSavedValue<matjson::Value>(id);

    if (!jsonConfig.contains(color)) {
        return getDefaultConfig(colorType);
    }

    config = configFromObject(jsonConfig[color]);

    return config;
}

Gradient Utils::getGradient(IconType type, bool secondPlayer) {
    Gradient gradient = {
        getSavedConfig(type, ColorType::Main, secondPlayer),
        getSavedConfig(type, ColorType::Secondary, secondPlayer),
        getSavedConfig(type, ColorType::Glow, secondPlayer)
    };

    if (secondPlayer && Utils::isSettingEnabled(P2_FLIP)
            && !Utils::isSettingEnabled(P2_SEPARATE)) {
        GradientConfig tempConfig = gradient.main;
        gradient.main = gradient.secondary;
        gradient.secondary = tempConfig;
    }

    return gradient;
}

void Utils::setIconColors(SimplePlayer* icon, ColorType colorType, bool white, bool secondPlayer) {
    GameManager* gm = GameManager::get();

    cocos2d::ccColor3B color1 = white ? ccc3(255, 255, 255)
        : gm->colorForIdx(gm->getPlayerColor());

    cocos2d::ccColor3B color2 = white ? ccc3(255, 255, 255)
        : gm->colorForIdx(gm->getPlayerColor2());
    
    bool hasGlowOutline = gm->getPlayerGlow();
    cocos2d::ccColor3B colorGlow = white ? ccc3(255, 255, 255)
        : gm->colorForIdx(gm->getPlayerGlowColor());
    
    if (secondPlayer) {
        if (Mod* sdiMod = Loader::get()->getLoadedMod("weebify.separate_dual_icons")) {
            color1 = gm->colorForIdx(sdiMod->getSavedValue<int>("color1", 0));
            color2 = gm->colorForIdx(sdiMod->getSavedValue<int>("color2", 0));
            hasGlowOutline = sdiMod->getSavedValue<bool>("glow", false);
            colorGlow = gm->colorForIdx(sdiMod->getSavedValue<int>("colorglow", 0));
        } else {
            cocos2d::ccColor3B tmpColor = color1;
            color1 = color2;
            color2 = tmpColor;
        }
    }

    icon->setColor(color1);
    icon->setSecondColor(color2);

    icon->m_hasGlowOutline = hasGlowOutline;

    if (icon->m_hasGlowOutline)
        icon->enableCustomGlowColor(colorGlow);
    else
        icon->disableCustomGlowColor();

    icon->updateColors();
}

std::string Utils::getTypeID(IconType type) {
    switch (type) {
        case IconType::Cube: return "cube";
        case IconType::Ship: return "ship";
        case IconType::Ball: return "ball";
        case IconType::Ufo: return "ufo";
        case IconType::Wave: return "wave";
        case IconType::Robot: return "robot";
        case IconType::Spider: return "spider";
        case IconType::Swing: return "swing";
        case IconType::Jetpack: return "jetpack";
        default: return "global";
    }
}

std::string Utils::getTypeID(SpriteType type) {
    switch (type) {
        case SpriteType::Icon: return "icon";
        case SpriteType::Vehicle: return "vehicle";
        case SpriteType::Animation: return "animation";
        default: return "icon";
    }
}

IconType Utils::getIconType(SimplePlayer* icon) {
    return static_cast<ProSimplePlayer*>(icon)->m_fields->m_type; 
}

std::vector<GradientConfig> Utils::getSavedGradients() {
    std::vector<GradientConfig> ret;

    for (const matjson::Value obj : Mod::get()->getSavedValue<matjson::Value>("saved-gradients"))
        ret.push_back(configFromObject(obj));

    return ret;
}

void Utils::applyGradient(SimplePlayer* icon, GradientConfig config, ColorType colorType, IconType iconType, bool secondPlayer, bool force, bool blend) {
    GJRobotSprite* otherSprite = nullptr;

    std::optional<std::tuple<ColorType, IconType, bool, unsigned int>> colorCacheKey =
        std::make_tuple(colorType, iconType, secondPlayer, 0);

    if (icon->m_robotSprite) if (icon->m_robotSprite->isVisible()) otherSprite = icon->m_robotSprite;
    if (icon->m_spiderSprite) if (icon->m_spiderSprite->isVisible()) otherSprite = icon->m_spiderSprite;

    if (otherSprite) {
        int partIndex = 0;
        switch (colorType) {
            case ColorType::Main:
                for (CCSpritePart* spr : CCArrayExt<CCSpritePart*>(otherSprite->m_headSprite->getParent()->getChildren())) {
                    if (!typeinfo_cast<CCSpritePart*>(spr)) continue;

                    if (colorCacheKey.has_value())
                        std::get<3>(colorCacheKey.value()) = partIndex;

                    applyGradient(spr, config, colorCacheKey, force, blend);
                    partIndex++;
                }
            break;
            case ColorType::Secondary:
                for (CCSprite* spr : CCArrayExt<CCSprite*>(otherSprite->m_secondArray)) {
                    if (!typeinfo_cast<CCSprite*>(spr) || spr == otherSprite->m_headSprite) continue;

                    if (colorCacheKey.has_value())
                        std::get<3>(colorCacheKey.value()) = partIndex;

                    applyGradient(spr, config, colorCacheKey, force, blend);
                    partIndex++;
                }
            break;
            case ColorType::Glow:
                for (CCSprite* spr : CCArrayExt<CCSprite*>(otherSprite->m_glowSprite->getChildren())) {
                    if (!typeinfo_cast<CCSpritePart*>(spr)) continue;

                    if (colorCacheKey.has_value())
                        std::get<3>(colorCacheKey.value()) = partIndex;
                    
                    applyGradient(spr, config, colorCacheKey, force, blend);
                    partIndex++;
                }
            break;
        }
    } else {
        CCSprite* sprite = nullptr;

        switch (colorType) {
            case ColorType::Main:
                sprite = icon->m_firstLayer;
                break;
            case ColorType::Secondary:
                sprite = icon->m_secondLayer;
                break;
            case ColorType::Glow:
                sprite = icon->m_outlineSprite;
                break;
        }

        applyGradient(sprite, config, colorCacheKey, force);
    }
}

void Utils::applyGradient(CCSprite* sprite, GradientConfig config, std::optional<std::tuple<ColorType, IconType, bool, unsigned int>> cacheKey, bool force, bool blend) {
    if (!sprite) return;
    
    CCGLProgram* defaultProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);

    if (config.points.empty())
        return sprite->setShaderProgram(defaultProgram);

    CCGLProgram* program = sprite->getShaderProgram();
     
    if (program == defaultProgram || force || blend) {
        std::string shaderFile = fmt::format("{}_gradient{}.fsh",
                                       config.isLinear ? "linear" : "radial",
                                       blend ? "_blend" : "");

        std::string vertPath = (Mod::get()->getResourcesDir() / "position.vert").string();
        std::string shaderPath =
            (Mod::get()->getResourcesDir() / shaderFile).string();

        if (!std::filesystem::exists(vertPath) || !std::filesystem::exists(shaderPath))
            return;

        std::string fullCacheKey;
        if (cacheKey.has_value()) {
            std::tuple<ColorType, IconType, bool, unsigned int> cacheKeyValue = cacheKey.value();
            fullCacheKey = getFullCacheKey(
                shaderFile,
                std::get<0>(cacheKeyValue),
                std::get<1>(cacheKeyValue),
                std::get<2>(cacheKeyValue),
                std::get<3>(cacheKeyValue)
            );

            CCGLProgram* cacheProgram = CCShaderCache::sharedShaderCache()->programForKey(fullCacheKey.c_str());
            if (cacheProgram != nullptr) {
                sprite->setShaderProgram(cacheProgram);
                return;
            }
        }

        program = new CCGLProgram();
        program->autorelease();

        program->initWithVertexShaderFilename(vertPath.c_str(), shaderPath.c_str());

        program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);

        program->link();
        program->updateUniforms();

        sprite->setShaderProgram(program);

        if (cacheKey.has_value())
            CCShaderCache::sharedShaderCache()->addProgram(program, fullCacheKey.c_str());
    }

    program->use();
    program->setUniformsForBuiltins();

    CCSpriteFrame* frame = sprite->displayFrame();
    CCRect rectInPixels = frame->getRectInPixels();
    CCSize texSize = frame->getTexture()->getContentSizeInPixels();

    bool rot = frame->m_bRotated;

    float uMin = rectInPixels.origin.x / texSize.width;
    float vMin = rectInPixels.origin.y / texSize.height;
    float uMax = (rectInPixels.origin.x + rectInPixels.size.width) / texSize.width;
    float vMax = (rectInPixels.origin.y + rectInPixels.size.height) / texSize.height;

    GLint locMin = glGetUniformLocation(program->getProgram(), "uvMin");
    GLint locMax = glGetUniformLocation(program->getProgram(), "uvMax");
    glUniform2f(locMin, uMin, vMin);
    glUniform2f(locMax, uMax, vMax);

    int stopAt = config.points.size();

    std::vector<cocos2d::ccColor4F> colors;

    for (const SimplePoint& point : config.points)
        colors.push_back(ccc4FFromccc3B(point.color));

    if (config.isLinear) {
        cocos2d::CCPoint startPoint = ccp(0, 0);
        cocos2d::CCPoint endPoint = ccp(0, 0);

        float distance = 0.f;

        for (const SimplePoint& point : config.points) {
            float currentDistance = ccpDistance(point.pos, {0.5f, 0.5f});
            if (currentDistance > distance) {
                startPoint = point.pos;
                distance = currentDistance;
            }
        }

        distance = 0.f;

        for (const SimplePoint& point : config.points) {
            float currentDistance = ccpDistance(point.pos, startPoint);
            if (currentDistance > distance) {
                endPoint = point.pos;
                distance = currentDistance;
            }
        }

        std::vector<float> stops;
        distance = ccpDistance(startPoint, endPoint);

        for (const SimplePoint& point : config.points)
            stops.push_back(ccpDistance(point.pos, startPoint) / distance);

        for (size_t i = 0; i < stops.size(); ++i)
            for (size_t j = i + 1; j < stops.size(); ++j)
                if (stops[i] > stops[j]) {
                    std::swap(stops[i], stops[j]);
                    std::swap(colors[i], colors[j]);
                }

        GLint startPointLoc = glGetUniformLocation(program->getProgram(), "startPoint");
        GLint endPointLoc   = glGetUniformLocation(program->getProgram(), "endPoint");

        glUniform2f(startPointLoc, rot ? startPoint.y : startPoint.x, rot ? startPoint.x : (1 - startPoint.y));
        glUniform2f(endPointLoc, rot ? endPoint.y : endPoint.x, rot ? endPoint.x : (1 - endPoint.y));

        GLint stopsLoc = glGetUniformLocation(program->getProgram(), "stops");
        glUniform1fv(stopsLoc, stopAt, stops.data());
    } else {
        std::vector<float> positions;

        for (const SimplePoint& point : config.points) {
            if (rot) {
                positions.push_back(point.pos.y);
                positions.push_back(point.pos.x);
            } else {
                positions.push_back(point.pos.x);
                positions.push_back(1 - point.pos.y);
            }
        }

        GLint loc = glGetUniformLocation(program->getProgram(), "positions");
        glUniform2fv(loc, stopAt, positions.data());
    }

    GLint stopAtLoc = glGetUniformLocation(program->getProgram(), "stopAt");
    glUniform1i(stopAtLoc, stopAt);

    std::vector<GLfloat> colorsData;
    for (const cocos2d::ccColor4F& color : colors) {
        colorsData.push_back(color.r);
        colorsData.push_back(color.g);
        colorsData.push_back(color.b);
        colorsData.push_back(color.a);
    }

    GLint colorsLoc = glGetUniformLocation(program->getProgram(), "colors");
    glUniform4fv(colorsLoc, stopAt, colorsData.data());
}

std::string Utils::getFullCacheKey(std::string shaderFile, ColorType colorType, IconType iconType, bool secondPlayer, unsigned int partIndex) {
    const char* iconTypeStr;
    const char* colorTypeStr;
    switch (iconType) {
    case IconType::Cube:
        iconTypeStr = "Cube";
        break;
    case IconType::Ship:
        iconTypeStr = "Ship";
        break;
    case IconType::Ball:
        iconTypeStr = "Ball";
        break;
    case IconType::Ufo:
        iconTypeStr = "Ufo";
        break;
    case IconType::Wave:
        iconTypeStr = "Wave";
        break;
    case IconType::Robot:
        iconTypeStr = "Robot";
        break;
    case IconType::Spider:
        iconTypeStr = "Spider";
        break;
    case IconType::Swing:
        iconTypeStr = "Swing";
        break;
    case IconType::Jetpack:
        iconTypeStr = "Jetpack";
        break;
    default:
        iconTypeStr = "Unknown";
        break;
    }
    switch (colorType) {
    case Main:
        colorTypeStr = "Main";
        break;
    case Secondary:
        colorTypeStr = "Secondary";
        break;
    case Glow:
        colorTypeStr = "Glow";
        break;
    }
    return fmt::format("{}-{}{}{}{}", shaderFile, secondPlayer ? "P2" : "P1", iconTypeStr, colorTypeStr, partIndex);
}

void Utils::patchBatchNode(CCSpriteBatchNode* node) {
    if (!node) return;
    
    static void* vtable = []() -> void* {
        FakeSpriteBatchNode temp;
        return *(void**)&temp;
    }();

    *(void**)node = vtable;
}
