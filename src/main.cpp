#include "./includes.h"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelTools.hpp>

#include "./MainLevelsCustomizerManager.h"

#include "./ErrorsManager/ErrorsManager.h"

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		MainLevelsCustomizerManager::getInstance();// to get all the json errors
		ErrorsManager::getInstance()->onMenuLayer(this);
		return true;
	}
};

class $modify(LevelSelectLayer) {
	bool init(int page) {
		MainLevelsCustomizerManager::getInstance()->onLevelSelectLayer_init();
		if (!LevelSelectLayer::init(page)) return false;
		return true;
	}

	void onBack(CCObject* sender) {
		LevelSelectLayer::onBack(sender);
		MainLevelsCustomizerManager::getInstance()->onLevelSelectLayer_onBack();
	}
};

class $modify(LevelPage) {
	void updateDynamicPage(GJGameLevel* level) {
		MainLevelsCustomizerManager::getInstance()->onLevelPage_customSetup(this, level);
		LevelPage::updateDynamicPage(level);
	}
};

class $modify(LevelTools) {
	static gd::string getAudioFileName(int id) {
		auto ret = LevelTools::getAudioFileName(id);
		MainLevelsCustomizerManager::getInstance()->onGetAudioFileName(ret, id);
		return ret;
	}
};