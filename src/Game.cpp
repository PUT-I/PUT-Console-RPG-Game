#include "Game.hpp"

using namespace std;

Game::Game() noexcept {
	//Settings loading -----------------------
	if (!files::exists("GameFiles\\settings.json")) {
		adaptFontSize();
		language = "en";
		codePage = "1250";
		currentScenario = "defaultScenario";
		saveSettings();
	}
	else {
		loadSettings();
	}
	adaptLocationFontSize();
	loadScenario();
	
	playing = true;
	inSession = false;

	Fullscreeen(true);
	refreshCodePage();
	ShowConsoleCursor(false);
	logo();
}

/*------------------------------ Functions ------------------------------*/
//Safeguards and initialization ----------
const bool Game::nameUsed(const string &name) {
	string temp;

	//Checking if Used -----------------------
	for (auto &p : filesystem::directory_iterator("Saves\\"+currentScenario)) {
		if (name + ".json" == p.path().filename().string()) {
			return true;
		}
	}
	return false;
}
const unsigned int Game::countSaves() {
	unsigned int counter = 0;
		if (filesystem::is_empty("Saves\\" + currentScenario) != true) {
			for (auto &p : filesystem::directory_iterator("Saves\\" + currentScenario)) {
				counter++;
			}
		}
	return counter;
}
void Game::loadResourceMaps() {
	//Object Maps loading --------------------
	{
		itms::weapons.clear();
		itms::armors.clear();
		itms::spells.clear();
		itms::consumables.clear();
		enemies.clear();

		json items;
		std::ifstream in("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\items.json");
		in >> items;
		in.close();

		files::loadMap(items["weapons"], itms::weapons);
		files::loadMap(items["armors"], itms::armors);
		files::loadMap(items["spells"], itms::spells);
		files::loadMap(items["consumables"], itms::consumables);

		//Enemies loading ------------------------
		json enemyTab;
		Enemy enemTemp;

		in.open("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\enemies.json");
		in >> enemyTab;
		in.close();
		for (const json &e : enemyTab["tab"]) {
			enemTemp.load(e["enemyInfo"]);
			enemies[e["key"]] = enemTemp;
		}
	}
}
void Game::initialize() {
	string name;

	do {
		//Name Setting ---------------------------
		ClearConsole();	
		PrintBox(0, 0, 54, 17);
		PrintText(3, 2, sLineTexts["Initialize1"]);
	
		InputString(name, 15);	//Input with letter limit
		
		if (nameUsed(name)) {
			PrintText(3, 4, sLineTexts["Initialize2"]);
			CursorPos(27 + sLineTexts["Initialize2"].size() / 6, 5);
			SpacePause();
			ClearConsole();
			continue;
		}
		
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		CursorPos(3, 2);
	} while (nameUsed(name) || !confirmationMenu(sLineTexts["Initialize3-1"] + name + sLineTexts["Initialize3-2"]));
	
	//Hero Initializing ---------------------
	this->hero.initialize(name);

	hero.addToInventory(itms::spells["Fire I"]);
	hero.addToInventory(itms::spells["Heal I"]);

	inSession = true;
}

//Saving and Loading ---------------------
void Game::load(const string &heroName) {
	hero.load(heroName);
}
void Game::deleteSavesMenu() {
	do {
	vector<string> usedNames;
	string temp;
	unsigned int choice = 0;
		if (countSaves() > 0) {
			for (auto &p : filesystem::directory_iterator("Saves\\" + currentScenario)) {
				temp = p.path().filename().string();
				temp.erase(temp.size() - 5, 5);
				usedNames.push_back(temp);
			}
			sort(usedNames.begin(), usedNames.end());
		}
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		optionsMenu(choice, sLineTexts["DeleteSavesMenuTitle"], sLineTexts["Return"], usedNames, true);

		if (choice == 0) return;
		else {
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			CursorPos(3, 2);
			if (confirmationMenu(sLineTexts["MainMenuQuestion"])) {
				filesystem::remove("Saves\\" + usedNames[choice - 1] + ".json");
			}
		}
	} while (true);
}
void Game::heroToTempHero() {
	tempHero = hero;
	vector<shared_ptr<Item>> tempInv;
	
	/*
	This part is needed in order to prevent reducing number
	of items after copying hero to tempHero, which may
	occur beacuse of the smart pointer usage
	*/
	for (shared_ptr<Item> &e : hero.getInventory()) {
		if (e->getType() == "weapon") {
			tempInv.push_back(make_shared<Weapon>(*dynamic_pointer_cast<Weapon>(e)));
		}
		else if (e->getType() == "armor") {
			tempInv.push_back(make_shared<Armor>(*dynamic_pointer_cast<Armor>(e)));
		}
		else if (e->getType() == "consumable") {
			tempInv.push_back(make_shared<Consumable>(*dynamic_pointer_cast<Consumable>(e)));
		}
		else if (e->getType() == "spell") {
			tempInv.push_back(make_shared<Spell>(*dynamic_pointer_cast<Spell>(e)));
		}
	}
}

//Battle System --------------------------
const bool Game::spellsExecution(const unsigned int & choice, const shared_ptr<Item> &item, Enemy &enemy) {
	bool result = false;
	shared_ptr<Character> enemyPtr = make_shared<Enemy>(enemy);

	CursorPosBegin();

	//if (choice > 0) audio.setVolume("enter", 0);
	
	if (hero.getMp() < item->getMana()) {
		result = false;
	}
	else if (item->getEffect() == "heal" && hero.getHp() >= hero.getHpMax()) {
		result = false;
	}
	else if (item->getEffect() == "stamina" && hero.getSp() >= hero.getSpMax()) {
		result = false;
	}
	else {
		hero.castSpell(*dynamic_pointer_cast<Spell>(item), enemyPtr);
		result = true;
	}

	enemy = *dynamic_pointer_cast<Enemy>(enemyPtr);

	return result;
}
void Game::displayFightStats(const Enemy& enemy) {
	//Stats Hero ----------------------------
	{
		CursorPos(1, 1);
		cout << hero.getName();
		CursorPos(1, 2);
		cout << color.get("red")	<< mLineTexts["FightStats"][0] << hero.getHp() << "/" << hero.getHpMax() << color.get("normal") << "  ";
		CursorPos(1, 3);
		cout << color.get("green")	<< mLineTexts["FightStats"][1] << hero.getSp() << "/" << hero.getSpMax() << color.get("normal") << "  ";
		CursorPos(1, 4);
		cout << color.get("blue")	<< mLineTexts["FightStats"][2] << hero.getMp() << "/" << hero.getMpMax() << color.get("normal") << "  ";
		CursorPos(1, 5);
		cout << mLineTexts["FightStats"][3] << hero.getDefence() << "  ";
		CursorPos(1, 6);
		cout << mLineTexts["FightStats"][4] << hero.getEvasionC() << "  ";
		CursorPos(1, 7);
		for (int i = 1; i <= 22; i++) {
			cout << "=";
		}
	}
	//Stats Enemy ----------------------------
	{
		CursorPos(1, 8);
		cout << enemy.loadName();
		CursorPos(1, 9);
		cout << color.get("red") << mLineTexts["FightStats"][0] << enemy.getHp() << "/" << enemy.getHpMax() << color.get("normal") << "  ";
		CursorPos(1, 10);
		cout << mLineTexts["FightStats"][3] << enemy.getDefence() << "  ";
		CursorPos(1, 11);
		cout << mLineTexts["FightStats"][4] << enemy.getEvasionC() << "  ";
	}
}

void Game::fightExecution(Enemy &enemy) {
	enemy.refreshStatDependent();

	const int choice = randInt(0, 1);

	switch (choice) {
	case 0: {
		int chance = enemy.getHitC() - hero.getEvasionC();
		if (chance < 1) chance = 1;

		if (randInt(1, 100) >= chance) {
			int damage = randInt(enemy.getDmgMin(), enemy.getDmgMax());

			if (damage < enemy.getDmgMin()) {
				damage = enemy.getDmgMin();
			}

			if (damage - hero.getDefence()> 0) {
				hero.getDamaged(damage);
				audio.creature(enemy.getSfxDir(), "hit");
			}
			else audio.miss();
		}
		else {
			audio.miss();
		}
		break;
	}

	case 1: {
		enemy.increaseDef(randInt(1, hero.getDmgMin() - enemy.getDefence()));
		audio.defend();
		break;
	}

	}
}
const bool Game::fightExecution(const unsigned int& choice, Enemy &enemy) {
	hero.refreshStatDependent();
	hero.refreshItemDependent();

	switch (choice) {
	case 0: {
		int chance = hero.getHitC() - enemy.getEvasionC();
		if (chance < 1) chance = 1;

		if ( randInt(1, 100) <=  chance) {
			int damage = randInt( (hero.getSp() / hero.getSpMax())*hero.getDmgMax(), hero.getDmgMax() );

			if ( damage < hero.getDmgMin() ) {
				damage = hero.getDmgMin();
			}

			if (damage - enemy.getDefence() > 0) {
				enemy.getDamaged(damage);
				audio.weapon(hero.getWeapon().getSfxDir());
			}
			else {
				audio.miss();
			}
		}
		else {
			audio.miss();
		}

		int dec = hero.getWeapon().getRequirements() * 10 - 2 * hero.getAgility();
		if (dec < 1) {
			dec = 1;
		}

		hero.decreaseSp(dec);
		break;
	}

	case 1: {
		hero.increaseDef( randInt(1, enemy.getDmgMin()-hero.getDefence()) );

		int dec = hero.getArmor().getRequirements() * 5 - 2 * hero.getAgility();
		if (dec < 1) {
			dec = 1;
		}

		hero.decreaseSp(dec);
		audio.defend();
		break;
	}

	case 2: {
		hero.increaseEv( randInt(1, 2*hero.getAgility()) );

		int dec = hero.getArmor().getRequirements() * 10 - 2 * hero.getAgility();
		if (dec < 1) {
			dec = 1;
		}

		hero.decreaseSp(dec);
		audio.evasion();
		break;
	}

	case 3: {
		ClearConsole();

		const bool temp = spellsMenu(enemy);

		ClearConsole();

		DisplayGfx(25, 1, graphics[enemy.getGfxDir()]);
		displayOptionsNoNum(choice, mLineTexts["Fight"], 2, 13);
		PrintBox(0, 0, 54, 17);
		PrintBox(0, 0, 22, 11);
		PrintBox(0, 12, 22, 5);

		if (temp == false) {
			return false;
		}

		break;
	}

	case 4: {
		ClearConsole();
		consumablesMenu();
		ClearConsole();

		DisplayGfx(25, 1, graphics[enemy.getGfxDir()]);
		PrintBox(0, 0, 54, 17);
		PrintBox(0, 0, 22, 11);
		PrintBox(0, 12, 22, 5);

		return false;
	}
	}

	return true;
}

void Game::fight(Enemy enemy){
	unsigned int choice = 0;

	ClearConsole();
	setFontSize(globalFontSize + 32);

	DisplayGfx(25, 1, graphics[enemy.getGfxDir()]);

	PrintBox(0, 0, 54, 17);
	PrintBox(0, 0, 22, 11);
	PrintBox(0, 12, 22, 5);

	do {
		do {
			displayFightStats(enemy);
			do {
				choiceLimit(choice, 5);
				if (choice >= 0 && choice <= 5) {
					displayOptionsNoNum(choice, mLineTexts["Fight"], 2, 13);
				}
			} while (!upDownControls(choice, 5));

		} while(!fightExecution(choice, enemy));

		enemy.activeSpellsRefresh();
		hero.activeSpellsRefresh();

		displayFightStats(enemy);

		if (enemy.getHp() <= 0) break;

		Sleep(500);
		fightExecution(enemy);

		hero.addSp(int{ hero.getSpMax()/100 });

	} while ( hero.getHp() > 0 && enemy.getHp() > 0);

	enemy.clearActiveSpells();
	hero.clearActiveSpells();

	if (hero.getHp() == 0) {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		CursorPos(8 , 4);
		cout << sLineTexts["MsgDead"];
		audio.creature("Character", "death");
		CursorPos(8, 6);
		SpacePause();
		ClearConsole();
		inSession = false;
	}
	else {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		CursorPos(8, 4);
		cout << sLineTexts["MsgWin"];
		CursorPos(8, 6);
		SpacePause();
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		hero.addExp(enemy.getExp());
	}

	setFontSize(globalFontSize + storyFontInc);
}

//Story ----------------------------------
void Game::printMenuGFX(const vector<string> &description, const vector<string> &gfx) {
	DisplayGfx(31, 1, description);
	DisplayGfx(1, 1, vector<string>(20, string(29, ' ')));
	DisplayGfx(1, 1, gfx);
	cout << color.get("normal");
	DisplayGfx(30, 1, lines);
	PrintBox(0, 0, 81, 26);
	PrintText(1, y1 - 1, equalSigns);
};

void Game::locationMenu(unsigned int &choice, const vector<string> &description, const vector<string> &options, const vector<string> &gfx) {
	int tempControlsResult = NULL;
	printMenuGFX(description, gfx);

	do {
		CursorPos(0, y1);
		displayOptionsMenu(choice, options);
		choiceLimit(choice, options.size());
		tempControlsResult = upDownEscControls(choice, options.size());
		if (tempControlsResult == -1) {
			audio.ambience(false);
			pauseMenu();
			if (!inSession) {
				return;
			}		
		audio.ambience(true);
		printMenuGFX(description, gfx);
		}
	} while (tempControlsResult != 1);
}
void Game::locationActionGfx(const vector<string> &description) {
	DisplayGfx(31, 1, locationTextBlank);
	DisplayGfx(1, 22, locationOptionsBlank);
	DisplayGfx(31, 1, description);

	CursorPos(1, 22);
	SpacePause();
	DisplayGfx(31, 1, locationTextBlank);
	DisplayGfx(1, 22, locationOptionsBlank);
}
void Game::jsonToStory() {
	ClearConsole();
	setFontSize(globalFontSize + storyFontInc);

	json subLocation;

	unsigned int choice;
	vector<string>action, description, options;
	auto parseAction = [this, &choice, &action, &description, &options]() {
		unsigned int statementTrueSize;
		for (unsigned int i = 0; i < action.size() && inSession; i++) {
				 if (action[i] == "sleep") { i++; Sleep(stoi(action[i])); }
			else if (action[i] == "break") { return true; }
			else if (action[i] == "clear") { ClearConsole(); }
			else if (action[i] == "sessionEnd") { inSession = false; }
			else if (action[i] == "pause") {
				unsigned int xPos, yPos;
				i++;
				xPos = stoi(action[i]);
				i++;
				yPos = stoi(action[i]);
				CursorPos(xPos, yPos);
				SpacePause();
			}
			else if (action[i] == "frame") { 
				i++;
				if (action[i] == "small") { setFontSize(globalFontSize + 32); PrintBox(0, 0, 54, 17); }
				else if (action[i] == "big") { setFontSize(globalFontSize + storyFontInc); PrintBox(0, 0, 81, 26); }
			}
			else if (action[i] == "text") {
				unsigned int xPos, yPos;
				i++;
				xPos = stoi(action[i]);
				i++;
				yPos = stoi(action[i]);
				i++;
				PrintText(xPos, yPos, sLineTexts[action[i]]);
			}
			else if (action[i] == "plotSwitch") {
				i++;
				unsigned int index = stoi(action[i]);
				i++;
				if (action[i] == "true") { 
					this->plotSwitches[index] = true;}
				else if (action[i] == "false") { this->plotSwitches[index] = false; }
			}
			else if (action[i] == "audio") {
				i++;
				if (action[i] == "creature") {
					audio.creature(action[i + 1], action[i + 2]);
					i += 2;
				}
				else if (action[i] == "character") {
					i++;
					audio.character(action[i]);
				}
				else if (action[i] == "enviroment") {
					i++;
					audio.enviroment(action[i]);
				}
				else if (action[i] == "misc") {
					i++;
					audio.misc(action[i]);
				}
				else if (action[i] == "ambience") {
					bool onOff;
					if (action[i+2] == "true") { onOff = true; }
					else { onOff = false; }
					audio.ambience(action[i + 1], onOff);
					i += 2;
				}
			}
			else if (action[i] == "choice") {
				i++;
				if (action[i] == "--") { choice--; }
				else if (action[i] == "++") { choice++; }
			}
			else if (action[i] == "add") {
				i++;
				if (action[i] == "item") {
					i++;
					if (action[i] == "weapon") { i++; hero.addToInventory(itms::weapons[action[i]]); }
					else if (action[i] == "armor") { i++; hero.addToInventory(itms::armors[action[i]]); }
					else if (action[i] == "spell") { i++; hero.addToInventory(itms::spells[action[i]]); }
					else if (action[i] == "consumable") { hero.addToInventory(itms::consumables[action[i + 1]], stoi(action[i + 2])); i += 2; }
				}
				else if (action[i] == "hp")		{ i++; hero.addHp(stoi(action[i]));		}
				else if (action[i] == "sp")		{ i++; hero.addSp(stoi(action[i]));		}
				else if (action[i] == "mp")		{ i++; hero.addMp(stoi(action[i]));		}
				else if (action[i] == "exp")	{ i++; hero.addExp(stoi(action[i]));	}
				else if (action[i] == "money")	{ i++; hero.addMoney(stoi(action[i]));	}
			}
			else if (action[i] == "setSubLocation") { i++; hero.setSubLocation(stoi(action[i])); }
			else if (action[i] == "setLocation") { i++; hero.setLocation(stoi(action[i])); }
			else if (action[i] == "fight") { i++; fight(enemies[action[i]]); }
			else if (action[i] == "printMenuGfx") { i++, printMenuGFX(vector<string>{""}, graphics[action[i]]); }
			else if (action[i] == "actionText") {
				i++;
				locationActionGfx(mLineTexts[action[i]]);
			}
			else if (action[i] == "description") {
				i++;
				description = mLineTexts[action[i]];
			}
			else if (action[i] == "optionsPopBack") { options.pop_back(); choice--; }
			else if (action[i] == "merchant") { 
				i++;
				shoppingChoiceMenu(files::load("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\merchants.json", action[i]));
			}
			else if (action[i] == "if") {		
				i++;
				if (action[i] == "plotSwitch") {
					i++;
					unsigned int index = stoi(action[i]);
					i++;
					statementTrueSize = stoi(action[i]);
					if (plotSwitches[index] == true) {
						continue;
					}
					else {
						i += statementTrueSize + 1;	//1 is for the number of commands digit in the file
						continue;
					}
				}
			}
			else if (action[i] == "endTrue") {
				i++;
				i += stoi(action[i]) + 1;
			}
		}
		return false;
	};

	while (inSession) {
			choice = 0;
			subLocation = scenario["location" + to_string(hero.getLocation())][hero.getSubLocation() - 1];

			if(subLocation["description"] != "NONE") description = mLineTexts[subLocation["description"]];
			options = mLineTexts[subLocation["options"]];

			if (subLocation["save"] == true)heroToTempHero();
			DisplayGfx(31, 1, locationTextBlank);
			DisplayGfx(1, 22, locationOptionsBlank);
				
			action = subLocation["preOptions"].get<vector<string>>();
			parseAction();

			while (true) {			
				if (subLocation["ambience"][0].get<string>() != "NONE") {
					audio.ambience(100.0f);
					audio.ambience(subLocation["ambience"][0].get<string>(), subLocation["ambience"][1].get<bool>());
				}
				else if (subLocation["ambience"][0].get<string>() == "NONE") {
					audio.ambience(0.0f);
					audio.ambience(subLocation["ambience"][1].get<bool>());
				}

				locationMenu(choice, description, options, graphics[subLocation["graphics"]]);
				if (inSession == false) {
					break;
				}

				action = subLocation["preChoice"].get<vector<string>>();
				parseAction();

				action = subLocation["choice"][choice]["action"].get<vector<string>>();
				if (parseAction())break;
				if (!inSession) {
					break;
				}
			}
			action = subLocation["afterChoice"].get<vector<string>>();
			parseAction();
		}

	audio.ambience(false);
	audio.mainMenu(true);
	audio.otherClear();
	setFontSize(globalFontSize + 32);
}