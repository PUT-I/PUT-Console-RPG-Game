#include "Game.hpp"
#include "CManip.hpp"
#include "Sounds.hpp"

using namespace std;

//Main and Pause Menus -------------------
const bool Game::pauseMenuExecution(const unsigned int &choice) {
	switch (choice) {
	case 0: {
		ClearConsole();
		return true;
	}

	case 1: {
		inventoryMenu();
		return false;
	}

	case 2: {
		ClearConsole();
		setFontSize(globalFontSize + 26);

		hero.display();
		SpacePause();

		ClearConsole();
		setFontSize(globalFontSize + 32);
		return false;
	}

	case 3: {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		PrintText(8, 2, sLineTexts["PauseMenuSave"]);
		tempHero.save();
		CursorPos(8, 4);
		SpacePause();
		return false;
	}

	case 4: {
		inSession = false;
		ClearConsole();
		return true;
	}

	case 5: {
		inSession = false;
		playing = false;
		ClearConsole();
		return true;
	}

	default: {
		return false;
	}
	}
}
void Game::pauseMenu() {
	ClearConsole();
	setFontSize(globalFontSize + 32);
	unsigned int choice = 0;

	//Displaying Until Continue or Exit ------
	do {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		optionsMenu(choice, sLineTexts["PauseMenuTitle"], mLineTexts["PauseMenu"], true);
	} while (!pauseMenuExecution(choice));
	setFontSize(globalFontSize + storyFontInc);
}

void Game::refreshCodePage() {
	string command = "chcp " + codePage;
	system(command.c_str());
	ClearConsole();
}
void Game::saveSettings() {
	json settings{
		{ "fontSize", globalFontSize },
		{ "greyscale", color.getGreyscale() },
		{ "sound", audio.getEnabled() },
		{ "scenario", currentScenario },
		{ "localization",{
			{ "language", language },
			{ "codePage", codePage }
			}
		}
	};

	ofstream out("GameFiles\\settings.json");
	out << setw(2) << settings;
	out.close();
}
void Game::loadSettings() {
	json settings;
	ifstream in("GameFiles\\settings.json");
	in >> settings;
	in.close();

	globalFontSize = settings["fontSize"];
	color.setGreyscale(settings["greyscale"]);
	audio.setEnabled(settings["sound"]);
	currentScenario = settings["scenario"].get<string>();
	language = settings["localization"]["language"].get<string>();
	codePage = settings["localization"]["codePage"].get<string>();
	audio.mainMenu(true);
}
void Game::settingsMenu() {
	ClearConsole();

	unsigned int choice = 0;
	int action;

	vector<string> languages, codePages;
	{
		string sTemp;
		ifstream in("GameFiles\\Localization\\languages.txt");

		while (in >> sTemp) {	//Loads language
			if (find(scenario["languages"].begin(), scenario["languages"].end(), sTemp) != scenario["languages"].end()) {
				languages.push_back(sTemp);
				in >> sTemp;		//Loads codepage
				codePages.push_back(sTemp);
			}
			else { in >> sTemp; } //Skips the codepage
		}
		in.close();
	}

	int choiceLang = NULL;
	int yesNo1;	//Greyscale
	int yesNo2;	//Sound

	if (!color.getGreyscale()) yesNo1 = 0;
	else yesNo1 = 1;

	if (!audio.getEnabled()) yesNo2 = 0;
	else yesNo2 = 1;

	for (unsigned int i = 0; i < languages.size(); i++) {
		if (languages[i] == language) choiceLang = i;
	}

	const vector<int*> numbers{
		&choiceLang,
		&globalFontSize,
		&yesNo1,
		&yesNo2
	};
	const vector<int> numbersLimit{
		(int)languages.size() - 1,
		60,
		1,
		1
	};

	const unsigned int ceil{ mLineTexts["Settings"].size() + 1 };

	auto printFrame = [](const string& title_) {
		cout << color.get("red");
		PrintBox(0, 0, 54, 17);
		cout << color.get("normal");
		PrintText(28 - title_.size() / 2, 0, title_);
	};

	printFrame(sLineTexts["SettingsTitle"]);

	do {
		//Limits --------------------------------
		{
			if (globalFontSize < 0) globalFontSize = 0;
			else if (globalFontSize > 60) globalFontSize = 60;

			if (choiceLang < 0) choiceLang = 0;
			else if (choiceLang == (int)languages.size()) choiceLang--;
		}

		if (choice == 1 && language != languages[choiceLang]) {
			language = languages[choiceLang];
			codePage = codePages[choiceLang];
			refreshCodePage();

			files::loadLocalization();
			printFrame(sLineTexts["SettingsTitle"]);
		}
		else if (choice == 2) {
			setFontSize(globalFontSize + 32);
		}
		else if (choice == 3) {
			int compareValue;
			if (color.getGreyscale()) { compareValue = 1; }
			else { compareValue = 0; }

			if (yesNo1 != compareValue) {
				if (yesNo1 == 0) { color.setGreyscale(false); }
				else if (yesNo1 == 1) { color.setGreyscale(true); }
				files::loadGraphics();
				printFrame(sLineTexts["SettingsTitle"]);
			}
		}
		else if (choice == 4) {
			if (yesNo2 == 0) { audio.setEnabled(false); }
			else if (yesNo2 == 1) { audio.setEnabled(true); }
			audio.mainMenu(true);
		}

		if (choice >= 0 && choice < ceil) {
			CursorPos(8, 4);
			cout << ((choice == 0) ? "> " : "  ") << sLineTexts["Return"];

			CursorPos(8, 6);
			cout << ((choice == 1) ? "> " : "  ") << mLineTexts["Settings"][0] << (languages[*numbers[0]]) << "   ";

			CursorPos(8, 8);
			cout << ((choice == 2) ? "> " : "  ") << mLineTexts["Settings"][1] << *(numbers[1]) << "   ";

			CursorPos(8, 10);
			cout << ((choice == 3) ? "> " : "  ") << mLineTexts["Settings"][2] << (color.getGreyscale() ? sLineTexts["Yes"] : sLineTexts["No"]) << "   ";

			CursorPos(8, 12);
			cout << ((choice == 4) ? "> " : "  ") << mLineTexts["Settings"][3] << (audio.getEnabled() ? sLineTexts["Yes"] : sLineTexts["No"]) << "   ";
		}
		choiceLimit(choice, ceil);

		action = upDownLeftRightEscControls(choice, ceil, numbers, numbersLimit);
	} while (action == 0);

	saveSettings();
}

void Game::loadScenario() {
	if (files::exists("GameFiles\\Scenarios\\" + currentScenario + "\\story.json")) {
		ifstream in("GameFiles\\Scenarios\\" + currentScenario + "\\story.json");
		in >> scenario;
		in.close();
	}
	else {
		ifstream in("GameFiles\\Scenarios\\DefaultScenario\\story.json");
		in >> scenario;
		in.close();
	}

	if (find(scenario["languages"].begin(), scenario["languages"].end(), language) == scenario["languages"].end()) {
		language = "en";
	}

	files::loadLocalization();
	sLineTexts["MainMenuTitle"] = "--- " + scenario[language + "Name"].get<string>() + " ---";

	loadResourceMaps();
	files::loadGraphics();
	audio.mainMenuReload();
	audio.mainMenu(true);
	if (!files::exists("Saves\\" + currentScenario)) filesystem::create_directory("Saves\\" + currentScenario);
}
void Game::scenarioLoadMenu() {
	do {
		vector<string> scenarios, scenarioNames;
		string temp;
		unsigned int choice = 0;

		auto find = [](const vector<string> &source, const string &toFind) {
			bool found = false;
			for (const string &s : source) {
				if (s == toFind) { found = true; break; }
			}
			return found;
		};

		for (auto &p : filesystem::directory_iterator("GameFiles\\Scenarios")) {
			temp = p.path().filename().string();
			scenarios.push_back(temp);
		}
		for (const string &s : scenarios) {
			json story;
			ifstream in("GameFiles\\Scenarios\\" + s + "\\story.json");
			in >> story;

			if (find(story["languages"].get<vector<string>>(), language) == true) {
				temp = story[language + "Name"].get<string>();
			}
			else {
				temp = story["enName"].get<string>();
			}
			if (find(scenario["languages"].get<vector<string>>(), language) == true) {
				if (temp == scenario[language + "Name"]) { temp += " (" + sLineTexts["ScenariosActive"] + ")"; }
			}
			else {
				if (temp == scenario["enName"]) temp += " (" + sLineTexts["ScenariosActive"] + ")";
			}
			scenarioNames.push_back(temp);
			in.close();
		}

		ClearConsole();
		PrintBox(0, 0, 54, 17);
		optionsMenu(choice, sLineTexts["ScenariosMenuTitle"], sLineTexts["Return"], scenarioNames, true);

		if (choice == 0) return;
		else {
			currentScenario = scenarios[choice - 1];
			loadScenario();
		}
	} while (true);
}

void Game::mainMenuExecution(unsigned int &choice) {
	switch (choice) {
	case 0: {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		CursorPos(3, 2);
		if (countSaves() >= 6) {
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			CursorPos(8, 4);
			cout << color.get("red") << sLineTexts["MsgSaves"] << color.get("normal");
			CursorPos(8, 6);
			SpacePause();
			ClearConsole();
		}
		else if (confirmationMenu(sLineTexts["MainMenuQuestion"])) {
			ClearConsole();
			initialize();
			audio.mainMenu(false);

			jsonToStory();
			if (!playing) return;
			hero.clear();
			choice = 0;
		}
		break;
	}
	case 1: {
		inSession = true;

		loadMenu();

		if (!inSession) break;
		else {
			jsonToStory();
			choice = 0;
		}

		hero.clear();

		if (!playing) return;
		break;
	}
	case 2: {
		deleteSavesMenu();
		break;
	}
	case 3: {
		scenarioLoadMenu();
		break;
	}
	case 4: {
		settingsMenu();
		break;
	}
	case 5: {
		playing = false;
		break;
	}

	default:
		break;
	}
}
void Game::logo() const
{
	sf::SoundBuffer tempBuffer;
	sf::Sound tempSound;
	tempSound.setBuffer(tempBuffer);
	ClearConsole();
	setFontSize(globalFontSize + 32);

	unsigned int x1 = 7, x2 = x1 + 15, x3 = x2 + 14;	//x1 - R, x2 - P, x3 - G
	unsigned int y1 = 3;


	Sleep(1000);

	cout << color.get("red");
	// Letter R
	{
		// R - roof
		PrintText(x1 + 1, y1, "_");
		PrintText(x1 + 2, y1, "_");
		PrintText(x1 + 3, y1, "_");
		PrintText(x1 + 4, y1, "_");
		PrintText(x1 + 5, y1, "_");
		PrintText(x1 + 6, y1, "_");

		// R - ceil
		PrintText(x1 + 3, y1 + 1, "_");
		PrintText(x1 + 4, y1 + 1, "_");
		PrintText(x1 + 5, y1 + 1, "_");

		// R - inner bottom
		PrintText(x1 + 3, y1 + 5, "_");
		PrintText(x1 + 4, y1 + 5, "_");
		PrintText(x1 + 5, y1 + 5, "_");

		// R - outer bottom
		PrintText(x1 + 3, y1 + 6, "_");
		PrintText(x1 + 4, y1 + 6, "_");

		PrintText(x1 + 1, y1 + 10, "_");
		PrintText(x1 + 9, y1 + 10, "_");

		// R - outer left side
		PrintText(x1, y1 + 1, "|");
		PrintText(x1, y1 + 2, "|");
		PrintText(x1, y1 + 3, "|");
		PrintText(x1, y1 + 4, "|");
		PrintText(x1, y1 + 5, "|");
		PrintText(x1, y1 + 6, "|");
		PrintText(x1, y1 + 7, "|");
		PrintText(x1, y1 + 8, "|");
		PrintText(x1, y1 + 9, "|");
		PrintText(x1, y1 + 10, "|");

		// R - inner left side
		PrintText(x1 + 2, y1 + 2, "|");
		PrintText(x1 + 2, y1 + 3, "|");
		PrintText(x1 + 2, y1 + 4, "|");
		PrintText(x1 + 2, y1 + 5, "|");

		PrintText(x1 + 2, y1 + 7, "|");
		PrintText(x1 + 2, y1 + 8, "|");
		PrintText(x1 + 2, y1 + 9, "|");
		PrintText(x1 + 2, y1 + 10, "|");

		// R - inner right side
		PrintText(x1 + 6, y1 + 2, "\\");
		PrintText(x1 + 7, y1 + 3, "\\");
		PrintText(x1 + 7, y1 + 4, "/");
		PrintText(x1 + 6, y1 + 5, "/");

		PrintText(x1 + 5, y1 + 7, "\\");
		PrintText(x1 + 6, y1 + 8, "\\");
		PrintText(x1 + 7, y1 + 9, "\\");
		PrintText(x1 + 8, y1 + 10, "\\");

		// R - outer right side
		PrintText(x1 + 7, y1 + 1, "\\");
		PrintText(x1 + 8, y1 + 2, "\\");
		PrintText(x1 + 9, y1 + 3, "\\");
		PrintText(x1 + 9, y1 + 4, "/");
		PrintText(x1 + 8, y1 + 5, "/");
		PrintText(x1 + 7, y1 + 6, "/");

		PrintText(x1 + 7, y1 + 7, "\\");
		PrintText(x1 + 8, y1 + 8, "\\");
		PrintText(x1 + 9, y1 + 9, "\\");
		PrintText(x1 + 10, y1 + 10, "\\");
	}
	cout << color.get("normal");

	tempBuffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Creatures\\Wolf\\Howls1.ogg");
	tempSound.play();
	Sleep(2000);

	tempBuffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Items\\Weapons\\Sword1.ogg");
	tempSound.play();
	// Letter P
	{
		// P - roof
		PrintText(x2 + 1, y1, "_");
		PrintText(x2 + 2, y1, "_");
		PrintText(x2 + 3, y1, "_");
		PrintText(x2 + 4, y1, "_");
		PrintText(x2 + 5, y1, "_");
		PrintText(x2 + 6, y1, "_");

		// P - ceil
		PrintText(x2 + 3, y1 + 1, "_");
		PrintText(x2 + 4, y1 + 1, "_");
		PrintText(x2 + 5, y1 + 1, "_");

		// P - inner bottom
		PrintText(x2 + 3, y1 + 5, "_");
		PrintText(x2 + 4, y1 + 5, "_");
		PrintText(x2 + 5, y1 + 5, "_");

		// P - outer bottom
		PrintText(x2 + 3, y1 + 6, "_");
		PrintText(x2 + 4, y1 + 6, "_");
		PrintText(x2 + 5, y1 + 6, "_");
		PrintText(x2 + 6, y1 + 6, "_");

		PrintText(x2 + 1, y1 + 10, "_");

		// P - outer left side
		PrintText(x2, y1 + 1, "|");
		PrintText(x2, y1 + 2, "|");
		PrintText(x2, y1 + 3, "|");
		PrintText(x2, y1 + 4, "|");
		PrintText(x2, y1 + 5, "|");
		PrintText(x2, y1 + 6, "|");
		PrintText(x2, y1 + 7, "|");
		PrintText(x2, y1 + 8, "|");
		PrintText(x2, y1 + 9, "|");
		PrintText(x2, y1 + 10, "|");

		// P - inner left side
		PrintText(x2 + 2, y1 + 2, "|");
		PrintText(x2 + 2, y1 + 3, "|");
		PrintText(x2 + 2, y1 + 4, "|");
		PrintText(x2 + 2, y1 + 5, "|");

		PrintText(x2 + 2, y1 + 7, "|");
		PrintText(x2 + 2, y1 + 8, "|");
		PrintText(x2 + 2, y1 + 9, "|");
		PrintText(x2 + 2, y1 + 10, "|");

		// P - inner right side
		PrintText(x2 + 6, y1 + 2, "\\");
		PrintText(x2 + 7, y1 + 3, "\\");
		PrintText(x2 + 7, y1 + 4, "/");
		PrintText(x2 + 6, y1 + 5, "/");

		// P - outer right side
		PrintText(x2 + 7, y1 + 1, "\\");
		PrintText(x2 + 8, y1 + 2, "\\");
		PrintText(x2 + 9, y1 + 3, "\\");
		PrintText(x2 + 9, y1 + 4, "/");
		PrintText(x2 + 8, y1 + 5, "/");
		PrintText(x2 + 7, y1 + 6, "/");
	}
	Sleep(1000);

	tempBuffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Creatures\\Character\\death.ogg");
	tempSound.play();
	cout << color.get("grey");
	// Letter G
	{
		// G - roof
		PrintText(x3 + 2, y1, "_");
		PrintText(x3 + 3, y1, "_");
		PrintText(x3 + 4, y1, "_");
		PrintText(x3 + 5, y1, "_");
		PrintText(x3 + 6, y1, "_");
		PrintText(x3 + 7, y1, "_");
		PrintText(x3 + 8, y1, "_");
		PrintText(x3 + 9, y1, "_");

		PrintText(x3 + 6, y1 + 5, "_");
		PrintText(x3 + 7, y1 + 5, "_");
		PrintText(x3 + 8, y1 + 5, "_");
		PrintText(x3 + 9, y1 + 5, "_");

		// G - ceil
		PrintText(x3 + 3, y1 + 1, "_");
		PrintText(x3 + 4, y1 + 1, "_");
		PrintText(x3 + 5, y1 + 1, "_");
		PrintText(x3 + 6, y1 + 1, "_");
		PrintText(x3 + 7, y1 + 1, "_");
		PrintText(x3 + 8, y1 + 1, "_");
		PrintText(x3 + 9, y1 + 1, "_");

		PrintText(x3 + 6, y1 + 6, "_");
		PrintText(x3 + 7, y1 + 6, "_");

		// G - inner bottom
		PrintText(x3 + 4, y1 + 9, "_");
		PrintText(x3 + 5, y1 + 9, "_");
		PrintText(x3 + 6, y1 + 9, "_");
		PrintText(x3 + 7, y1 + 9, "_");

		// G - outer bottom
		PrintText(x3 + 3, y1 + 10, "_");
		PrintText(x3 + 4, y1 + 10, "_");
		PrintText(x3 + 5, y1 + 10, "_");
		PrintText(x3 + 6, y1 + 10, "_");
		PrintText(x3 + 7, y1 + 10, "_");
		PrintText(x3 + 8, y1 + 10, "_");
		PrintText(x3 + 9, y1 + 10, "_");

		// G - outer left side
		PrintText(x3 + 1, y1 + 1, "/");
		PrintText(x3, y1 + 2, "/");
		PrintText(x3, y1 + 3, "|");
		PrintText(x3, y1 + 4, "|");
		PrintText(x3, y1 + 5, "|");
		PrintText(x3, y1 + 6, "|");
		PrintText(x3, y1 + 7, "|");
		PrintText(x3, y1 + 8, "|");
		PrintText(x3 + 1, y1 + 9, "\\");
		PrintText(x3 + 2, y1 + 10, "\\");

		// G - inner left side
		PrintText(x3 + 2, y1 + 2, "/");
		PrintText(x3 + 2, y1 + 3, "|");
		PrintText(x3 + 2, y1 + 4, "|");
		PrintText(x3 + 2, y1 + 5, "|");
		PrintText(x3 + 2, y1 + 6, "|");
		PrintText(x3 + 2, y1 + 7, "|");
		PrintText(x3 + 2, y1 + 8, "\\");
		PrintText(x3 + 3, y1 + 9, "\\");

		// G - inner right side
		PrintText(x3 + 5, y1 + 6, "|");
		PrintText(x3 + 8, y1 + 7, "|");
		PrintText(x3 + 8, y1 + 8, "|");
		PrintText(x3 + 8, y1 + 9, "|");

		// G - outer right side
		PrintText(x3 + 10, y1 + 1, "|");
		PrintText(x3 + 10, y1 + 6, "|");
		PrintText(x3 + 10, y1 + 7, "|");
		PrintText(x3 + 10, y1 + 8, "|");
		PrintText(x3 + 10, y1 + 9, "|");
		PrintText(x3 + 10, y1 + 10, "/");
	}
	cout << color.get("normal");
	Sleep(1500);

	tempBuffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Fight\\Miss1.ogg");
	tempSound.play();
	PrintBox(0, 0, 54, 17);
	Sleep(3000);
}
void Game::mainMenu() {
	setFontSize(globalFontSize + 32);
	unsigned int choice = 0;
	vector<string> options;
	string title;

	while (playing) {
		//Displaying Menu Options ----------------
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		optionsMenu(choice, sLineTexts["MainMenuTitle"], mLineTexts["MainMenu"], false);
		mainMenuExecution(choice);
	}
	saveSettings();
}

//Saving and Loading ---------------------
void Game::loadMenu() {
	vector<string> usedNames;
	unsigned int choice = 0;

	if (countSaves() > 0) {
		for (auto &p : filesystem::directory_iterator("Saves\\" + currentScenario)) {
			string temp = p.path().filename().string();
			temp.erase(temp.size() - 5, 5);
			usedNames.push_back(temp);
		}
		sort(usedNames.begin(), usedNames.end());
	}

	ClearConsole();
	PrintBox(0, 0, 54, 17);
	optionsMenu(choice, sLineTexts["LoadMenuTitle"], sLineTexts["Return"], usedNames, true);

	if (choice == 0) inSession = false;
	else {
		hero.clear();
		load(usedNames[choice - 1]);
		audio.mainMenu(false);
	}
}