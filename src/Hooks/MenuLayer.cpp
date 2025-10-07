#include "../Utils/Utils.hpp"

#include "MenuLayer.hpp"

bool ProMenuLayer::init() {
    if (!MenuLayer::init()) return false;

    if (!Loader::get()->isModLoaded("capeling.icon_profile")) return true;

    if (CCNode* spr = getChildByIDRecursive("profile-icon"))
        if (SimplePlayer* icon = spr->getChildByType<SimplePlayer>(0)) {
            IconType type = Utils::getIconType(icon);

            Gradient gradient = Utils::getGradient(type, false);

            Utils::applyGradient(icon, gradient.main, ColorType::Main, type, false, true);
            Utils::applyGradient(icon, gradient.secondary, ColorType::Secondary, type, false, true);
            Utils::applyGradient(icon, gradient.glow, ColorType::Glow, type, false, true);
        }

    return true;
}
