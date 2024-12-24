#include "./MainLevelsCustomizerManager.h"
#include "./FastPatch.h"

void MainLevelsCustomizerManager::init() {
	loadingStatus = loadData();
	if (loadingStatus == OK) {
		FastPatch::make("0x38F894", "EB");//load unfailed
		auto byteVector = std::vector<uint8_t>();
		byteVector.push_back(levelsData.size() + 1);
		FastPatch::patch(0x303DC5, byteVector);//levels count patch
		
		if (unlockDemons) {
			FastPatch::make("0x3064F4", "90 E9 DC 01 00 00");//remove locks
			FastPatch::make("0x308DAE", "90 E9 3D 01 00 00");//fix level starting
		}

		if (isHideTheTower)
			FastPatch::make("0x303F4A", "90 90 90 90 90 90 90 90 90 90 90 90");//hide The Tower
	}
}

MainLevelsCustomizerManager::DataLoadingResult MainLevelsCustomizerManager::loadData() {
	std::ifstream file("Resources/levelCustomizer.json");
	if (!file) return FileNotFound;
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string fileContent = buffer.str();

	file.close();
	try {
		auto root = nlohmann::json::parse(fileContent);

		if (!root.contains("settings") || !root["settings"].is_object() || !root.contains("levels") || !root["levels"].is_array()) return ParsingError;

		if (root["settings"].contains("unlockDemons") && root["settings"]["unlockDemons"].is_boolean() && root["settings"]["unlockDemons"].get<bool>()) unlockDemons = true;
		if (root["settings"].contains("hideTheTower") && root["settings"]["hideTheTower"].is_boolean() && root["settings"]["hideTheTower"].get<bool>()) isHideTheTower = true;

		if (root["levels"].size() < 1 || root["levels"].size() > 126) return LevelsCountError;

		for (auto level : root["levels"]) {
			if (!level.contains("name") || !level["name"].is_string()
				|| !level.contains("difficulty") || !level["difficulty"].is_number_integer()
				|| !level.contains("stars") || !level["stars"].is_number_integer()
				|| !level.contains("song") || !level["song"].is_string()) return ParsingError;

			auto newLevel = new MainLevelData();
			newLevel->name = level["name"];
			newLevel->difficulty = level["difficulty"].get<int>();
			newLevel->stars = level["stars"].get<int>();
			newLevel->song = level["song"];

			if (newLevel->name == "") newLevel->name = "ERROR :/";
			if (newLevel->difficulty < 1) newLevel->difficulty = 1;
			if (newLevel->difficulty > 6) newLevel->difficulty = 6;
			if (newLevel->stars < 1) newLevel->stars = 1;

			levelsData.push_back(newLevel);
		}
	}
	catch (...) {

		return ParsingError;
	}
	return OK;
}

void MainLevelsCustomizerManager::onMenuLayer(MenuLayer* layer) {

	if (loadingStatus != OK) {

		std::string errorText;
		switch (loadingStatus) {
		case MainLevelsCustomizerManager::FileNotFound:
			errorText = "Can't find 'levelCustomizer.json' in ./Resources";
			break;
		case MainLevelsCustomizerManager::ParsingError:
			errorText = "Can't parse 'levelCustomizer.json'";
			break;
		case MainLevelsCustomizerManager::LevelsCountError:
			errorText = "Too many or too few levels in 'levelCustomizer.json";
			break;
		}

		auto size = CCDirector::sharedDirector()->getWinSize();

		auto errorLabel = CCLabelBMFont::create(errorText.c_str(), "bigFont.fnt");
		errorLabel->setColor({ 255, 0, 0 });
		errorLabel->setScale(0.6);
		errorLabel->setPosition({ size.width / 2, size.height - 10 });
		layer->addChild(errorLabel);
	}
}

void MainLevelsCustomizerManager::onLevelPage_customSetup(void* self, GJGameLevel* lvl) {
	if (loadingStatus != OK || !isInMainLevels) return;
	
	if (lvl->m_levelID.value() > -1) {
		auto id = lvl->m_levelID.value();

		bool brokenValue = id < 1 || id > levelsData.size();

		lvl->m_levelName = brokenValue ? "Error :/" : levelsData[id - 1]->name;

		lvl->m_stars = brokenValue ? 999 : levelsData[id - 1]->stars;

		lvl->m_difficulty = static_cast<GJDifficulty>(brokenValue ? 1 : levelsData[id - 1]->difficulty);
	}
}

void MainLevelsCustomizerManager::onGetAudioFileName(std::string& self, int id) {
	if (loadingStatus != OK || !isInMainLevels) return;

	if (id < 0 || id > levelsData.size() - 1) return;

	self = levelsData[id]->song;
}

void MainLevelsCustomizerManager::onLevelSelectLayer_init() {
	isInMainLevels = true;
}

void MainLevelsCustomizerManager::onLevelSelectLayer_onBack() {
	isInMainLevels = false;
}


MainLevelsCustomizerManager* MainLevelsCustomizerManager::getInstance() {
	if (!instance) {
		instance = new MainLevelsCustomizerManager();
		instance->init();
	}
	return instance;
}

MainLevelsCustomizerManager* MainLevelsCustomizerManager::instance = nullptr;