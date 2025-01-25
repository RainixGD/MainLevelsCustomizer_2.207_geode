#pragma once
#include "./includes.h"

struct MainLevelData {
	std::string name;
	int difficulty;
	int stars;
	std::string song;
};



class MainLevelsCustomizerManager {

	bool isOk = false;

	std::vector<MainLevelData*> levelsData;
	bool unlockDemons = false;
	static MainLevelsCustomizerManager* instance;

	bool isInMainLevels = false;
	bool isHideTheTower = false;

	void init();
	bool loadData();

	MainLevelsCustomizerManager() {};
public:

	void onLevelPage_customSetup(void* self, GJGameLevel* lvl);
	void onGetAudioFileName(std::string& self, int id);

	void onLevelSelectLayer_init();
	void onLevelSelectLayer_onBack();

	static MainLevelsCustomizerManager* getInstance();
};