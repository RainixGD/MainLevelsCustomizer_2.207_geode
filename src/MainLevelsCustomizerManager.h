#pragma once
#include "./includes.h"

struct MainLevelData {
	std::string name;
	int difficulty;
	int stars;
	std::string song;
};



class MainLevelsCustomizerManager {

	enum DataLoadingResult {
		OK,
		FileNotFound,
		ParsingError,
		LevelsCountError,
	};

	std::vector<MainLevelData*> levelsData;
	bool unlockDemons = false;
	DataLoadingResult loadingStatus;
	static MainLevelsCustomizerManager* instance;

	bool isInMainLevels = false;
	bool isHideTheTower = false;

	void init();
	DataLoadingResult loadData();

	MainLevelsCustomizerManager() {};
public:

	void onMenuLayer(MenuLayer* layer);
	void onLevelPage_customSetup(void* self, GJGameLevel* lvl);
	void onGetAudioFileName(std::string& self, int id);

	void onLevelSelectLayer_init();
	void onLevelSelectLayer_onBack();

	static MainLevelsCustomizerManager* getInstance();
};