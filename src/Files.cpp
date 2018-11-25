#include "Files.hpp"
#include "CManip.hpp"
#include "Character.hpp"

using namespace std;

//Needed to display regional characters (nlohmann parses in UTF8 and this app uses ANSI)
string utf8_to_ansi(const string &str_, const vector<string> &vUtf, const vector<string> &vAnsi) {
	string str = str_;
	if (vUtf.size() > 0 && str_.size() > 0) {
		bool changed;
		do {
			changed = false;
			for (unsigned int i = 0; i < str.size() - 1; i++) {
				for (unsigned int j = 0; j < vUtf.size(); j++) {
					if (str[i] == vUtf[j][0] && str[i + 1] == vUtf[j][1]) {
						changed = true;
						str[i] = vAnsi[j][0];
						str.erase(str.begin() + i + 1);
					}
				}
			}
		} while (changed);
	}
	return str;
}
vector<string> utf8_to_ansi(const vector<string> &vStr_, const vector<string> &vUtf, const vector<string> &vAnsi) {
	vector<string> vStr;
	if (vUtf.size() > 0) {
		for (const string &e : vStr_) {
			vStr.push_back(utf8_to_ansi(e, vUtf, vAnsi));
		}
	}
	else vStr = vStr_;
	return vStr;
}

namespace files {
	//Loading txt ----------------------------
	const vector<string> loadGfx(const string &dir) {
		string temp;
		char cTemp;
		vector<string> result;
		ifstream in(dir);
		while (in.get(cTemp)) {
			if (cTemp == '\n') {
				result.push_back(temp);
				temp.clear();
			}
			else if (cTemp == '&') {
				string colorTemp;
				bool end;
				do {
					end = true;
					in.get(cTemp);
					if (cTemp >= '0' && cTemp <= '9' && cTemp != '&') {
						colorTemp.push_back(cTemp);
						end = false;
					}
				} while (!end);

				temp.append(color.get(colorTemp));

				if (cTemp != '&') temp.push_back(cTemp);
			}
			else temp.push_back(cTemp);
		}
		in.close();

		return result;
	}
	const vector<string> loadGfx(const vector<string> &input) {
		string temp;
		char cTemp;
		vector<string> result;

		for (const string &e : input) {
			istringstream in(e);
			while (in >> std::noskipws >> cTemp) {
				if (cTemp == '\n') {
					result.push_back(temp);
					temp.clear();
				}
				else if (cTemp == '&') {
					string colorTemp;
					bool end;
					do {
						end = true;
						in.get(cTemp);
						if (cTemp >= '0' && cTemp <= '9' && cTemp != '&') {
							colorTemp.push_back(cTemp);
							end = false;
						}
					} while (!end);

					temp.append(color.get(colorTemp));

					if (cTemp != '&') temp.push_back(cTemp);
				}
				else temp.push_back(cTemp);
			}
			result.push_back(temp);
			temp.clear();
		}
		return result;
	}
	const string proccessTxtLine(const string &line) {
		string temp;
		char cTemp;

		istringstream in(line);
		in >> std::noskipws;
		while (in.get(cTemp)) {
			if (cTemp == '&') {
				string colorTemp;
				do {
					in.get(cTemp);
					if (cTemp >= '0' && cTemp <= '9' && cTemp != '&') {
						colorTemp.push_back(cTemp);
					}
					else {
						temp += color.get(colorTemp);
						break;
					}
				} while (true);
				if (cTemp != '&') temp.push_back(cTemp);
			}
			else temp.push_back(cTemp);
		}
		return temp;
	}
	const vector<string> loadTxt(const string &dir) {
		vector<string> result;
		string sTemp;

		ifstream in(dir);

		while (getline(in, sTemp)) {
			result.push_back(sTemp);
		}

		in.close();
		return result;
	}
	const string loadLine(const string &dir) {
		string result;

		ifstream in(dir);
		getline(in, result);
		in.close();

		return result;
	}

	//Directories and Files ------------------
	void loadLocalization() {
		vector<string> vUtf, vAnsi;
		string temp;

		mLineTexts.clear();
		sLineTexts.clear();
		itemNames.clear();
		enemyNames.clear();

		ifstream in;

		//Loading table of regional characters ---
		in.open("GameFiles\\Localization\\" + language + "_utf_to_ansi_character_list.txt");
		while (in >> temp) {
			vUtf.push_back(temp);
			in >> temp;
			vAnsi.push_back(temp);
		}
		in.close();

		json loaded;
		vector<string> textDirs{
			"GameFiles\\Scenarios\\" + currentScenario + "\\Localization\\" + language + "\\Locations.json",
			"GameFiles\\Scenarios\\" + currentScenario + "\\Localization\\" + language + "\\Options.json",
			"GameFiles\\Localization\\" + language + "\\UI.json"
		};
		vector<string> itemTabNames{
			"weapons",
			"armors",
			"consumables",
			"spells"
		};

		//Texts ----------------------------------
		for (const string &dir : textDirs) {
			in.open(dir);
			in >> loaded;
			in.close();
			for (const json &e : loaded["sLine"]["tab"]) {
				sLineTexts[e["key"]] = files::proccessTxtLine(utf8_to_ansi(e["text"].get<string>(), vUtf, vAnsi));
			}
			for (const json &e : loaded["mLine"]["tab"]) {
				mLineTexts[e["key"]] = utf8_to_ansi(e["text"].get<vector<string>>(), vUtf, vAnsi);
			}
		}

		//Item names -----------------------------
		in.open("GameFiles\\Scenarios\\" + currentScenario + "\\Localization\\" + language + "\\Items.json");
		in >> loaded;
		in.close();
		for (const string &name : itemTabNames) {
			for (const json &e : loaded[name]["tab"]) {
				itemNames[e["key"]] = utf8_to_ansi(e["name"].get<string>(), vUtf, vAnsi);
			}
		}

		//Enemies names --------------------------
		in.open("GameFiles\\Scenarios\\" + currentScenario + "\\Localization\\" + language + "\\Enemies.json");
		in >> loaded;
		in.close();
		for (const json &e : loaded["tab"]) {
			enemyNames[e["key"]] = utf8_to_ansi(e["name"].get<string>(), vUtf, vAnsi);
		}
		in.close();

		//Action Texts ---------------------------
		in.open("GameFiles\\Scenarios\\" + currentScenario + "\\Localization\\" + language + "\\Action.json");
		in >> loaded;
		in.close();
		for (const json &e : loaded["tab"]) {
			mLineTexts[e["key"]] = utf8_to_ansi(files::loadGfx(e["text"].get<vector<string>>()), vUtf, vAnsi);
		}
	}

	const bool exists(const string &path) {
		return std::filesystem::exists(path);
	}
	void deleteAllSaves() {
		if (exists("Saves")) {
			filesystem::remove_all("Saves");
		}
		if (!exists("Saves")) {
			filesystem::create_directory("Saves");
		}
	}
	void createSaveDirectory() {
		if (filesystem::exists("Saves") == false) {
			filesystem::create_directory("Saves");
		}
	}
}