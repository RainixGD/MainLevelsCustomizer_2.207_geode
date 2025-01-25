#include "./MainLevelsCustomizerManager.h"
#include "./FastPatch.h"
#include "./ErrorsManager/ErrorsManager.h"

void MainLevelsCustomizerManager::init() {
	isOk = loadData();
	if (isOk) {
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

bool MainLevelsCustomizerManager::loadData() {
    std::ifstream file("Resources/levelCustomizer.json");
    if (!file) {
        ErrorsManager::addError("Main Levels Customizer: File 'Resources/levelCustomizer.json' not found or unable to open.", ErrorsManager::Error);
        return false;
    }

    std::ostringstream buffer;
    try {
        buffer << file.rdbuf();
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Main Levels Customizer: Failed to read from file 'Resources/levelCustomizer.json'. IOError: " + std::string(e.what()), ErrorsManager::Error);
        file.close();
        return false;
    }

    std::string fileContent = buffer.str();
    file.close();

    if (fileContent.empty()) {
        ErrorsManager::addError("Main Levels Customizer: File 'Resources/levelCustomizer.json' is empty.", ErrorsManager::Error);
        return false;
    }

    try {
        auto root = nlohmann::json::parse(fileContent);

        if (!root.contains("settings") || !root["settings"].is_object() || !root.contains("levels") || !root["levels"].is_array()) {
            ErrorsManager::addError("Main Levels Customizer: Missing required properties: 'settings' or 'levels'.", ErrorsManager::Error);
            return false;
        }

        if (root["settings"].contains("unlockDemons") && root["settings"]["unlockDemons"].is_boolean() &&
            root["settings"]["unlockDemons"].get<bool>()) {
            unlockDemons = true;
        }

        if (root["settings"].contains("hideTheTower") && root["settings"]["hideTheTower"].is_boolean() &&
            root["settings"]["hideTheTower"].get<bool>()) {
            isHideTheTower = true;
        }

        if (root["levels"].size() < 1 || root["levels"].size() > 126) {
            ErrorsManager::addError("Main Levels Customizer: Invalid number of levels. The count must be between 1 and 126.", ErrorsManager::Error);
            return false;
        }

        for (auto level : root["levels"]) {
            if (!level.contains("name") || !level["name"].is_string() ||
                !level.contains("difficulty") || !level["difficulty"].is_number_integer() ||
                !level.contains("stars") || !level["stars"].is_number_integer() ||
                !level.contains("song") || !level["song"].is_string()) {
                ErrorsManager::addError("Main Levels Customizer: Missing or invalid properties in level data.", ErrorsManager::Error);
                return false;
            }

            auto newLevel = new MainLevelData();
            newLevel->name = level["name"];
            newLevel->difficulty = level["difficulty"].get<int>();
            newLevel->stars = level["stars"].get<int>();
            newLevel->song = level["song"];

            if (newLevel->name.size() > 100 || newLevel->name.size() == 0) {
                ErrorsManager::addError("Main Levels Customizer: Level name must be between 1 and 100 characters.", ErrorsManager::Error);
                return false;
            }

            if (newLevel->difficulty < 1 || newLevel->difficulty > 6) {
                ErrorsManager::addError("Main Levels Customizer: Level difficulty must be between 1 and 6.", ErrorsManager::Error);
                return false;
            }

            if (newLevel->stars <= 0 || newLevel->stars > 9999) {
                ErrorsManager::addError("Main Levels Customizer: Level stars count must be between 1 and 9999.", ErrorsManager::Error);
                return false;
            }

            levelsData.push_back(newLevel);
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        ErrorsManager::addError("Main Levels Customizer: JSON parsing error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const nlohmann::json::type_error& e) {
        ErrorsManager::addError("Main Levels Customizer: JSON type error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Main Levels Customizer: I/O failure: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::exception& e) {
        ErrorsManager::addError("Main Levels Customizer: Unknown error: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }

    return true;
}

void MainLevelsCustomizerManager::onLevelPage_customSetup(void* self, GJGameLevel* lvl) {
	if (!isOk || !isInMainLevels) return;
	
	if (lvl->m_levelID.value() > -1) {
		auto id = lvl->m_levelID.value();

		bool brokenValue = id < 1 || id > levelsData.size();

		lvl->m_levelName = brokenValue ? "Error :/" : levelsData[id - 1]->name;

		lvl->m_stars = brokenValue ? 999 : levelsData[id - 1]->stars;

		lvl->m_difficulty = static_cast<GJDifficulty>(brokenValue ? 1 : levelsData[id - 1]->difficulty);
	}
}

void MainLevelsCustomizerManager::onGetAudioFileName(std::string& self, int id) {
	if (!isOk || !isInMainLevels) return;

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