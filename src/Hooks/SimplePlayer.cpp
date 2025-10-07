#include "../Utils/Utils.hpp"

#include "SimplePlayer.hpp"

void ProSimplePlayer::updatePlayerFrame(int p0, IconType type) {
	SimplePlayer::updatePlayerFrame(p0, type);

	m_fields->m_type = type;

	if (!Loader::get()->isModLoaded("ninkaz.colorful-icons") || GJBaseGameLayer::get()) return;

	retain();

	bool useP2Gradient = false;
	bool isGarage = CCDirector::sharedDirector()->getRunningScene()->getChildByType<GJGarageLayer>(0);
	if (isGarage)
		if (Mod* sdiMod = Loader::get()->getLoadedMod("weebify.separate_dual_icons"))
			useP2Gradient = sdiMod->getSavedValue<bool>("2pselected");

	Loader::get()->queueInMainThread([this, type, useP2Gradient, isGarage] {
		if (!getParent()) return;
		if (!typeinfo_cast<GJItemIcon*>(getParent())) return;

		if (CCSprite* spr = getChildByType<CCSprite>(0))
			if (spr->getOpacity() <= 120) return;

		Gradient gradient = Utils::getGradient(type, useP2Gradient);

		Utils::applyGradient(this, gradient.main, ColorType::Main, type, useP2Gradient, true, false, isGarage);
		Utils::applyGradient(this, gradient.secondary, ColorType::Secondary, type, useP2Gradient, true, false, isGarage);
		Utils::applyGradient(this, gradient.glow, ColorType::Glow, type, useP2Gradient, true, false, isGarage);

		autorelease();
	});
}
