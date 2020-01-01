#include "files_util.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "character.hpp"
#include "color_util.hpp"
#include "global_variables.hpp"

using namespace std;
using namespace nlohmann;

// Needed to display regional characters (nlohmann parses in UTF8 and this app uses ANSI)
string utf8_to_ansi(string str, const vector<string>& utf_vector, const vector<string>& ansi_vector)
{
	if (!utf_vector.empty() && !str.empty())
	{
		bool changed;
		do
		{
			changed = false;
			for (unsigned int i = 0; i < str.size() - 1; i++)
			{
				for (unsigned int j = 0; j < utf_vector.size(); j++)
				{
					if (str[i] == utf_vector[j][0] && str[i + 1] == utf_vector[j][1])
					{
						changed = true;
						str[i] = ansi_vector[j][0];
						str.erase(str.begin() + i + 1);
					}
				}
			}
		}
		while (changed);
	}
	return str;
}

vector<string> utf8_to_ansi(vector<string> string_vector, const vector<string>& utf_vector,
                            const vector<string>& ansi_vector)
{
	if (!utf_vector.empty())
	{
		for (const string& e : string_vector)
		{
			string_vector.push_back(utf8_to_ansi(e, utf_vector, ansi_vector));
		}
	}
	return string_vector;
}

namespace files
{
	// Loading txt ----------------------------
	const vector<string> load_gfx(const string& dir)
	{
		string temp;
		char char_temp;
		vector<string> result;
		ifstream in(dir);
		while (in.get(char_temp))
		{
			if (char_temp == '\n')
			{
				result.push_back(temp);
				temp.clear();
			}
			else if (char_temp == '&')
			{
				string color_temp;
				bool end;
				do
				{
					end = true;
					in.get(char_temp);
					if (char_temp >= '0' && char_temp <= '9' && char_temp != '&')
					{
						color_temp.push_back(char_temp);
						end = false;
					}
				}
				while (!end);

				temp.append(color.get(color_temp));

				if (char_temp != '&') temp.push_back(char_temp);
			}
			else temp.push_back(char_temp);
		}
		in.close();

		return result;
	}

	const vector<string> load_gfx(const vector<string>& input)
	{
		string temp;
		char char_temp;
		vector<string> result;

		for (const string& e : input)
		{
			istringstream in(e);
			while (in >> std::noskipws >> char_temp)
			{
				if (char_temp == '\n')
				{
					result.push_back(temp);
					temp.clear();
				}
				else if (char_temp == '&')
				{
					string color_temp;
					bool end;
					do
					{
						end = true;
						in.get(char_temp);
						if (char_temp >= '0' && char_temp <= '9' && char_temp != '&')
						{
							color_temp.push_back(char_temp);
							end = false;
						}
					}
					while (!end);

					temp.append(color.get(color_temp));

					if (char_temp != '&') temp.push_back(char_temp);
				}
				else temp.push_back(char_temp);
			}
			result.push_back(temp);
			temp.clear();
		}
		return result;
	}

	const string proccess_txt_line(const string& line)
	{
		string temp;
		char char_temp;

		istringstream in(line);
		in >> std::noskipws;
		while (in.get(char_temp))
		{
			if (char_temp == '&')
			{
				string color_temp;
				do
				{
					in.get(char_temp);
					if (char_temp >= '0' && char_temp <= '9' && char_temp != '&')
					{
						color_temp.push_back(char_temp);
					}
					else
					{
						temp += color.get(color_temp);
						break;
					}
				}
				while (true);
				if (char_temp != '&') temp.push_back(char_temp);
			}
			else temp.push_back(char_temp);
		}
		return temp;
	}

	const vector<string> load_txt(const string& dir)
	{
		vector<string> result;
		string string_temp;

		ifstream in(dir);

		while (getline(in, string_temp))
		{
			result.push_back(string_temp);
		}

		in.close();
		return result;
	}

	const string load_line(const string& dir)
	{
		string result;

		ifstream in(dir);
		getline(in, result);
		in.close();

		return result;
	}

	// Directories and Files ------------------
	void load_localization()
	{
		vector<string> utf_vector, ansi_vector;
		string temp;

		m_line_texts.clear();
		s_line_texts.clear();
		item_names.clear();
		enemy_names.clear();

		ifstream in;

		// Loading table of regional characters ---
		in.open("GameFiles\\Localization\\" + language + "_utf_to_ansi_character_list.txt");
		while (in >> temp)
		{
			utf_vector.push_back(temp);
			in >> temp;
			ansi_vector.push_back(temp);
		}
		in.close();

		json loaded;
		vector<string> text_dirs{
			"GameFiles\\Scenarios\\" + current_scenario + "\\Localization\\" + language + "\\Locations.json",
			"GameFiles\\Scenarios\\" + current_scenario + "\\Localization\\" + language + "\\Options.json",
			"GameFiles\\Localization\\" + language + "\\UI.json"
		};
		vector<string> item_tab_names{
			"weapons",
			"armors",
			"consumables",
			"spells"
		};

		// Texts ----------------------------------
		for (const string& dir : text_dirs)
		{
			in.open(dir);
			in >> loaded;
			in.close();
			for (const json& e : loaded["sLine"]["tab"])
			{
				s_line_texts.insert(
					std::pair(e["key"], files::proccess_txt_line(
						          utf8_to_ansi(e["text"].get<string>(), utf_vector, ansi_vector))));
			}
			for (const json& e : loaded["mLine"]["tab"])
			{
				m_line_texts.insert(
					std::pair(e["key"], utf8_to_ansi(e["text"].get<vector<string>>(), utf_vector, ansi_vector)));
			}
		}

		// Item names -----------------------------
		in.open("GameFiles\\Scenarios\\" + current_scenario + "\\Localization\\" + language + "\\Items.json");
		in >> loaded;
		in.close();
		for (const string& name : item_tab_names)
		{
			for (const json& e : loaded[name]["tab"])
			{
				item_names.insert(std::pair(e["key"], utf8_to_ansi(e["name"].get<string>(), utf_vector, ansi_vector)));
			}
		}

		// Enemies names --------------------------
		in.open("GameFiles\\Scenarios\\" + current_scenario + "\\Localization\\" + language + "\\Enemies.json");
		in >> loaded;
		in.close();
		for (const json& e : loaded["tab"])
		{
			enemy_names.insert(std::pair(e["key"], utf8_to_ansi(e["name"].get<string>(), utf_vector, ansi_vector)));
		}
		in.close();

		// Action Texts ---------------------------
		in.open("GameFiles\\Scenarios\\" + current_scenario + "\\Localization\\" + language + "\\Action.json");
		in >> loaded;
		in.close();
		for (const json& e : loaded["tab"])
		{
			m_line_texts.insert(
				std::pair(e["key"], utf8_to_ansi(files::load_gfx(e["text"].get<vector<string>>()), utf_vector,
				                                 ansi_vector)));
		}
	}

	const bool exists(const string& path)
	{
		return std::filesystem::exists(path);
	}

	void delete_all_saves()
	{
		if (exists("Saves"))
		{
			filesystem::remove_all("Saves");
		}
		if (!exists("Saves"))
		{
			filesystem::create_directory("Saves");
		}
	}

	void create_save_directory()
	{
		if (filesystem::exists("Saves") == false)
		{
			filesystem::create_directory("Saves");
		}
	}

	// Saving and loading collections ----------
	void save(ofstream& out, const vector<shared_ptr<item>>& inv)
	{
		const json size{
			{"inventorySize", inv.size()}
		};
		out << "\n\n" << size;

		for (const shared_ptr<item>& e : inv)
		{
			out << '\n';
			e->save(out);
		}
	}

	const vector<shared_ptr<item>> load_inventory(ifstream& in)
	{
		json item_info;
		vector<shared_ptr<item>> inv;
		weapon wep_temp;
		armor armor_temp;
		consumable consum_temp;
		spell spell_temp;

		json size;
		in >> size;
		for (int i = 0; i < size["inventorySize"]; i++)
		{
			in >> item_info;
			if (item_info["type"] == "weapon")
			{
				wep_temp.load(item_info);
				inv.push_back(make_shared<weapon>(wep_temp));
			}
			else if (item_info["type"] == "armor")
			{
				armor_temp.load(item_info);
				inv.push_back(make_shared<armor>(armor_temp));
			}
			else if (item_info["type"] == "consumable")
			{
				consum_temp.load(item_info);
				inv.push_back(make_shared<consumable>(consum_temp));
			}
			else if (item_info["type"] == "spell")
			{
				spell_temp.load(item_info);
				inv.push_back(make_shared<spell>(spell_temp));
			}

			item_info.clear();
		}

		return inv;
	}

	const vector<shared_ptr<item>> load_inventory(const string& file_dir, const string& which)
	{
		json file_inv;
		vector<shared_ptr<item>> inv;
		weapon wep_temp;
		armor armor_temp;
		consumable consum_temp;
		spell spell_temp;
		string category;

		ifstream in(file_dir);
		in >> file_inv;
		in.close();

		for (const json& j : file_inv["merchants"])
		{
			if (j["key"] == which)
			{
				for (const string& s : j.at("inventory"))
				{
					if (s == "consumables" || s == "weapons" || s == "armors" || s == "spells")
					{
						category = s;
					}
					else
					{
						if (category == "consumables")
						{
							inv.push_back(make_shared<consumable>(items::consumables[s]));
						}
						else if (category == "weapons") { inv.push_back(make_shared<weapon>(items::weapons[s])); }
						else if (category == "armors") { inv.push_back(make_shared<armor>(items::armors[s])); }
						else if (category == "spells") { inv.push_back(make_shared<spell>(items::spells[s])); }
					}
				}
				break;
			}
		}

		return inv;
	}

	void load_graphics()
	{
		graphics.clear();

		for (const auto& elem : enemies)
		{
			graphics[elem.second.get_gfx_dir()] = elem.second.load_gfx();
		}
		for (const auto& elem : items::weapons)
		{
			graphics[elem.second.get_gfx_dir()] = elem.second.load_gfx();
		}
		for (const auto& elem : items::armors)
		{
			graphics[elem.second.get_gfx_dir()] = elem.second.load_gfx();
		}
		for (const auto& elem : items::spells)
		{
			graphics[elem.second.get_gfx_dir()] = elem.second.load_gfx();
		}
		for (const auto& elem : items::consumables)
		{
			graphics[elem.second.get_gfx_dir()] = elem.second.load_gfx();
		}
		for (auto& p : filesystem::directory_iterator(
			     "GameFiles\\Scenarios\\" + current_scenario + R"(\Resources\Graphics\Locations)"))
		{
			std::ifstream in(p);
			string temp = p.path().filename().string();
			temp.erase(temp.size() - 4, 4);
			graphics[temp] = load_gfx(
				"GameFiles\\Scenarios\\" + current_scenario + R"(\Resources\Graphics\Locations\)" + p
				                                                                                    .path().filename().
				                                                                                    string());
			in.close();
		}
	}
}
