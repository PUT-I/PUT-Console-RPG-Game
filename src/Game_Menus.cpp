#include "game.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "color_util.hpp"
#include "console_util.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"
#include "interface_util.hpp"
#include "sound_manager.hpp"

using namespace std;
using namespace nlohmann;

// Main and Pause Menus -------------------
const bool game::pause_menu_execution(const unsigned int& choice)
{
	switch (choice)
	{
	case 0:
		clear_console();
		return true;
	case 1:
		inventory_menu();
		return false;
	case 2:
		clear_console();
		set_font_size(global_font_size + 26);

		hero_.display();
		space_pause();

		clear_console();
		set_font_size(global_font_size + 32);
		return false;
	case 3:
		clear_console();
		print_box(0, 0, 54, 17);
		print_text(8, 2, s_line_texts["PauseMenuSave"]);
		temp_hero_.save();
		cursor_pos(8, 4);
		space_pause();
		return false;
	case 4:
		in_session_ = false;
		clear_console();
		return true;
	case 5:
		in_session_ = false;
		playing_ = false;
		clear_console();
		return true;
	default:
		return false;
	}
}

void game::pause_menu()
{
	clear_console();
	set_font_size(global_font_size + 32);
	unsigned int choice = 0;

	// Displaying Until Continue or Exit ------
	do
	{
		clear_console();
		print_box(0, 0, 54, 17);
		options_menu(choice, s_line_texts["PauseMenuTitle"], m_line_texts["PauseMenu"], true);
	}
	while (!pause_menu_execution(choice));
	set_font_size(global_font_size + story_font_inc);
}

void game::refresh_code_page()
{
	const string command = "chcp " + code_page;
	system(command.c_str());
	clear_console();
}

void game::save_settings()
{
	const json settings{
		{"fontSize", global_font_size},
		{"greyscale", color.get_greyscale()},
		{"sound", sounds.get_enabled()},
		{"scenario", current_scenario},
		{
			"localization", {
				{"language", language},
				{"codePage", code_page}
			}
		}
	};

	ofstream out("GameFiles\\settings.json");
	out << setw(2) << settings;
	out.close();
}

void game::load_settings()
{
	json settings;
	ifstream in("GameFiles\\settings.json");
	in >> settings;
	in.close();

	global_font_size = settings["fontSize"];
	color.set_greyscale(settings["greyscale"]);
	sounds.set_enabled(settings["sound"]);
	current_scenario = settings["scenario"].get<string>();
	language = settings["localization"]["language"].get<string>();
	code_page = settings["localization"]["codePage"].get<string>();
	sounds.main_menu(true);
}

void game::settings_menu()
{
	clear_console();

	unsigned int choice = 0;
	int action;

	vector<string> languages, code_pages;
	{
		string str_temp;
		ifstream in("GameFiles\\Localization\\languages.txt");

		while (in >> str_temp)
		{
			// Loads language
			if (find(scenario_["languages"].begin(), scenario_["languages"].end(), str_temp) != scenario_["languages"].end())
			{
				languages.push_back(str_temp);
				in >> str_temp; // Loads codepage
				code_pages.push_back(str_temp);
			}
			else { in >> str_temp; } // Skips the codepage
		}
		in.close();
	}

	int choice_lang = NULL;
	int greyscale; // Greyscale
	int sound; // Sound

	if (!color.get_greyscale()) greyscale = 0;
	else greyscale = 1;

	if (!sounds.get_enabled()) sound = 0;
	else sound = 1;

	for (unsigned int i = 0; i < languages.size(); i++)
	{
		if (languages[i] == language) choice_lang = i;
	}

	const vector<int*> numbers{
		&choice_lang,
		&global_font_size,
		&greyscale,
		&sound
	};
	const vector<int> numbers_limit{
		int(languages.size()) - 1,
		60,
		1,
		1
	};

	const unsigned int ceil{m_line_texts["Settings"].size() + 1};

	auto print_frame = [](const string& title)
	{
		cout << color.get("red");
		print_box(0, 0, 54, 17);
		cout << color.get("normal");
		print_text(28 - title.size() / 2, 0, title);
	};

	print_frame(s_line_texts["SettingsTitle"]);

	do
	{
		// Limits --------------------------------
		{
			if (global_font_size < 0) global_font_size = 0;
			else if (global_font_size > 60) global_font_size = 60;

			if (choice_lang < 0) choice_lang = 0;
			else if (choice_lang == int(languages.size())) choice_lang--;
		}

		if (choice == 1 && language != languages[choice_lang])
		{
			language = languages[choice_lang];
			code_page = code_pages[choice_lang];
			refresh_code_page();

			files::load_localization();
			print_frame(s_line_texts["SettingsTitle"]);
		}
		else if (choice == 2)
		{
			set_font_size(global_font_size + 32);
		}
		else if (choice == 3)
		{
			int compare_value;
			if (color.get_greyscale()) { compare_value = 1; }
			else { compare_value = 0; }

			if (greyscale != compare_value)
			{
				if (greyscale == 0) { color.set_greyscale(false); }
				else if (greyscale == 1) { color.set_greyscale(true); }
				files::load_graphics();
				print_frame(s_line_texts["SettingsTitle"]);
			}
		}
		else if (choice == 4)
		{
			if (sound == 0) { sounds.set_enabled(false); }
			else if (sound == 1) { sounds.set_enabled(true); }
			sounds.main_menu(true);
		}

		if (choice >= 0 && choice < ceil)
		{
			cursor_pos(8, 4);
			cout << ((choice == 0) ? "> " : "  ") << s_line_texts["Return"];

			cursor_pos(8, 6);
			cout << ((choice == 1) ? "> " : "  ") << m_line_texts["Settings"][0] << (languages[*numbers[0]]) << "   ";

			cursor_pos(8, 8);
			cout << ((choice == 2) ? "> " : "  ") << m_line_texts["Settings"][1] << *(numbers[1]) << "   ";

			cursor_pos(8, 10);
			cout << ((choice == 3) ? "> " : "  ") << m_line_texts["Settings"][2] << (color.get_greyscale()
				                                                                         ? s_line_texts["Yes"]
				                                                                         : s_line_texts["No"]) << "   ";

			cursor_pos(8, 12);
			cout << ((choice == 4) ? "> " : "  ") << m_line_texts["Settings"][3] << (sounds.get_enabled()
				                                                                         ? s_line_texts["Yes"]
				                                                                         : s_line_texts["No"]) << "   ";
		}
		choice_limit(choice, ceil);

		action = up_down_left_right_esc_controls(choice, ceil, numbers, numbers_limit);
	}
	while (action == 0);

	save_settings();
}

void game::load_scenario()
{
	if (files::exists("GameFiles\\Scenarios\\" + current_scenario + "\\story.json"))
	{
		ifstream in("GameFiles\\Scenarios\\" + current_scenario + "\\story.json");
		in >> scenario_;
		in.close();
	}
	else
	{
		ifstream in("GameFiles\\Scenarios\\DefaultScenario\\story.json");
		in >> scenario_;
		in.close();
	}

	if (find(scenario_["languages"].begin(), scenario_["languages"].end(), language) == scenario_["languages"].end())
	{
		language = "en";
	}

	files::load_localization();
	s_line_texts["MainMenuTitle"] = "--- " + scenario_[language + "Name"].get<string>() + " ---";

	load_resource_maps();
	files::load_graphics();
	sounds.main_menu_reload();
	sounds.main_menu(true);
	if (!files::exists("Saves\\" + current_scenario)) filesystem::create_directory("Saves\\" + current_scenario);
}

void game::scenario_load_menu()
{
	do
	{
		vector<string> scenarios, scenario_names;
		string temp;
		unsigned int choice = 0;

		auto find = [](const vector<string>& source, const string& to_find)
		{
			bool found = false;
			for (const string& s : source)
			{
				if (s == to_find)
				{
					found = true;
					break;
				}
			}
			return found;
		};

		for (auto& p : filesystem::directory_iterator("GameFiles\\Scenarios"))
		{
			temp = p.path().filename().string();
			scenarios.push_back(temp);
		}
		for (const string& s : scenarios)
		{
			json story;
			ifstream in("GameFiles\\Scenarios\\" + s + "\\story.json");
			in >> story;

			if (find(story["languages"].get<vector<string>>(), language) == true)
			{
				temp = story[language + "Name"].get<string>();
			}
			else
			{
				temp = story["enName"].get<string>();
			}
			if (find(scenario_["languages"].get<vector<string>>(), language) == true)
			{
				if (temp == scenario_[language + "Name"]) { temp += " (" + s_line_texts["ScenariosActive"] + ")"; }
			}
			else
			{
				if (temp == scenario_["enName"]) temp += " (" + s_line_texts["ScenariosActive"] + ")";
			}
			scenario_names.push_back(temp);
			in.close();
		}

		clear_console();
		print_box(0, 0, 54, 17);
		options_menu(choice, s_line_texts["ScenariosMenuTitle"], s_line_texts["Return"], scenario_names, true);

		if (choice == 0) return;
		else
		{
			current_scenario = scenarios[choice - 1];
			load_scenario();
		}
	}
	while (true);
}

void game::main_menu_execution(unsigned int& choice)
{
	switch (choice)
	{
	case 0:
		{
			clear_console();
			print_box(0, 0, 54, 17);
			cursor_pos(3, 2);
			if (count_saves() >= 6)
			{
				clear_console();
				print_box(0, 0, 54, 17);
				cursor_pos(8, 4);
				cout << color.get("red") << s_line_texts["MsgSaves"] << color.get("normal");
				cursor_pos(8, 6);
				space_pause();
				clear_console();
			}
			else if (confirmation_menu(s_line_texts["MainMenuQuestion"]))
			{
				clear_console();
				initialize();
				sounds.main_menu(false);

				json_to_story();
				if (!playing_) return;
				hero_.clear();
				choice = 0;
			}
			break;
		}
	case 1:
		{
			in_session_ = true;

			load_menu();

			if (!in_session_) break;
			else
			{
				json_to_story();
				choice = 0;
			}

			hero_.clear();

			if (!playing_) return;
			break;
		}
	case 2:
		{
			delete_saves_menu();
			break;
		}
	case 3:
		{
			scenario_load_menu();
			break;
		}
	case 4:
		{
			settings_menu();
			break;
		}
	case 5:
		{
			playing_ = false;
			break;
		}

	default:
		break;
	}
}

void game::logo() const
{
	sf::SoundBuffer temp_buffer;
	sf::Sound temp_sound;
	temp_sound.setBuffer(temp_buffer);
	clear_console();
	set_font_size(global_font_size + 32);

	const unsigned int x1 = 7;
	const unsigned int x2 = x1 + 15;
	const unsigned int x3 = x2 + 14;
	const unsigned int y1 = 3;


	Sleep(1000);

	cout << color.get("red");
	// Letter R
	{
		// R - roof
		print_text(x1 + 1, y1, "_");
		print_text(x1 + 2, y1, "_");
		print_text(x1 + 3, y1, "_");
		print_text(x1 + 4, y1, "_");
		print_text(x1 + 5, y1, "_");
		print_text(x1 + 6, y1, "_");

		// R - ceil
		print_text(x1 + 3, y1 + 1, "_");
		print_text(x1 + 4, y1 + 1, "_");
		print_text(x1 + 5, y1 + 1, "_");

		// R - inner bottom
		print_text(x1 + 3, y1 + 5, "_");
		print_text(x1 + 4, y1 + 5, "_");
		print_text(x1 + 5, y1 + 5, "_");

		// R - outer bottom
		print_text(x1 + 3, y1 + 6, "_");
		print_text(x1 + 4, y1 + 6, "_");

		print_text(x1 + 1, y1 + 10, "_");
		print_text(x1 + 9, y1 + 10, "_");

		// R - outer left side
		print_text(x1, y1 + 1, "|");
		print_text(x1, y1 + 2, "|");
		print_text(x1, y1 + 3, "|");
		print_text(x1, y1 + 4, "|");
		print_text(x1, y1 + 5, "|");
		print_text(x1, y1 + 6, "|");
		print_text(x1, y1 + 7, "|");
		print_text(x1, y1 + 8, "|");
		print_text(x1, y1 + 9, "|");
		print_text(x1, y1 + 10, "|");

		// R - inner left side
		print_text(x1 + 2, y1 + 2, "|");
		print_text(x1 + 2, y1 + 3, "|");
		print_text(x1 + 2, y1 + 4, "|");
		print_text(x1 + 2, y1 + 5, "|");

		print_text(x1 + 2, y1 + 7, "|");
		print_text(x1 + 2, y1 + 8, "|");
		print_text(x1 + 2, y1 + 9, "|");
		print_text(x1 + 2, y1 + 10, "|");

		// R - inner right side
		print_text(x1 + 6, y1 + 2, "\\");
		print_text(x1 + 7, y1 + 3, "\\");
		print_text(x1 + 7, y1 + 4, "/");
		print_text(x1 + 6, y1 + 5, "/");

		print_text(x1 + 5, y1 + 7, "\\");
		print_text(x1 + 6, y1 + 8, "\\");
		print_text(x1 + 7, y1 + 9, "\\");
		print_text(x1 + 8, y1 + 10, "\\");

		// R - outer right side
		print_text(x1 + 7, y1 + 1, "\\");
		print_text(x1 + 8, y1 + 2, "\\");
		print_text(x1 + 9, y1 + 3, "\\");
		print_text(x1 + 9, y1 + 4, "/");
		print_text(x1 + 8, y1 + 5, "/");
		print_text(x1 + 7, y1 + 6, "/");

		print_text(x1 + 7, y1 + 7, "\\");
		print_text(x1 + 8, y1 + 8, "\\");
		print_text(x1 + 9, y1 + 9, "\\");
		print_text(x1 + 10, y1 + 10, "\\");
	}
	cout << color.get("normal");

	temp_buffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Creatures\\Wolf\\Howls1.ogg");
	temp_sound.play();
	Sleep(2000);

	temp_buffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Items\\Weapons\\Sword1.ogg");
	temp_sound.play();
	// Letter P
	{
		// P - roof
		print_text(x2 + 1, y1, "_");
		print_text(x2 + 2, y1, "_");
		print_text(x2 + 3, y1, "_");
		print_text(x2 + 4, y1, "_");
		print_text(x2 + 5, y1, "_");
		print_text(x2 + 6, y1, "_");

		// P - ceil
		print_text(x2 + 3, y1 + 1, "_");
		print_text(x2 + 4, y1 + 1, "_");
		print_text(x2 + 5, y1 + 1, "_");

		// P - inner bottom
		print_text(x2 + 3, y1 + 5, "_");
		print_text(x2 + 4, y1 + 5, "_");
		print_text(x2 + 5, y1 + 5, "_");

		// P - outer bottom
		print_text(x2 + 3, y1 + 6, "_");
		print_text(x2 + 4, y1 + 6, "_");
		print_text(x2 + 5, y1 + 6, "_");
		print_text(x2 + 6, y1 + 6, "_");

		print_text(x2 + 1, y1 + 10, "_");

		// P - outer left side
		print_text(x2, y1 + 1, "|");
		print_text(x2, y1 + 2, "|");
		print_text(x2, y1 + 3, "|");
		print_text(x2, y1 + 4, "|");
		print_text(x2, y1 + 5, "|");
		print_text(x2, y1 + 6, "|");
		print_text(x2, y1 + 7, "|");
		print_text(x2, y1 + 8, "|");
		print_text(x2, y1 + 9, "|");
		print_text(x2, y1 + 10, "|");

		// P - inner left side
		print_text(x2 + 2, y1 + 2, "|");
		print_text(x2 + 2, y1 + 3, "|");
		print_text(x2 + 2, y1 + 4, "|");
		print_text(x2 + 2, y1 + 5, "|");

		print_text(x2 + 2, y1 + 7, "|");
		print_text(x2 + 2, y1 + 8, "|");
		print_text(x2 + 2, y1 + 9, "|");
		print_text(x2 + 2, y1 + 10, "|");

		// P - inner right side
		print_text(x2 + 6, y1 + 2, "\\");
		print_text(x2 + 7, y1 + 3, "\\");
		print_text(x2 + 7, y1 + 4, "/");
		print_text(x2 + 6, y1 + 5, "/");

		// P - outer right side
		print_text(x2 + 7, y1 + 1, "\\");
		print_text(x2 + 8, y1 + 2, "\\");
		print_text(x2 + 9, y1 + 3, "\\");
		print_text(x2 + 9, y1 + 4, "/");
		print_text(x2 + 8, y1 + 5, "/");
		print_text(x2 + 7, y1 + 6, "/");
	}
	Sleep(1000);

	temp_buffer.loadFromFile(
		"GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Creatures\\Character\\death.ogg");
	temp_sound.play();
	cout << color.get("grey");
	// Letter G
	{
		// G - roof
		print_text(x3 + 2, y1, "_");
		print_text(x3 + 3, y1, "_");
		print_text(x3 + 4, y1, "_");
		print_text(x3 + 5, y1, "_");
		print_text(x3 + 6, y1, "_");
		print_text(x3 + 7, y1, "_");
		print_text(x3 + 8, y1, "_");
		print_text(x3 + 9, y1, "_");

		print_text(x3 + 6, y1 + 5, "_");
		print_text(x3 + 7, y1 + 5, "_");
		print_text(x3 + 8, y1 + 5, "_");
		print_text(x3 + 9, y1 + 5, "_");

		// G - ceil
		print_text(x3 + 3, y1 + 1, "_");
		print_text(x3 + 4, y1 + 1, "_");
		print_text(x3 + 5, y1 + 1, "_");
		print_text(x3 + 6, y1 + 1, "_");
		print_text(x3 + 7, y1 + 1, "_");
		print_text(x3 + 8, y1 + 1, "_");
		print_text(x3 + 9, y1 + 1, "_");

		print_text(x3 + 6, y1 + 6, "_");
		print_text(x3 + 7, y1 + 6, "_");

		// G - inner bottom
		print_text(x3 + 4, y1 + 9, "_");
		print_text(x3 + 5, y1 + 9, "_");
		print_text(x3 + 6, y1 + 9, "_");
		print_text(x3 + 7, y1 + 9, "_");

		// G - outer bottom
		print_text(x3 + 3, y1 + 10, "_");
		print_text(x3 + 4, y1 + 10, "_");
		print_text(x3 + 5, y1 + 10, "_");
		print_text(x3 + 6, y1 + 10, "_");
		print_text(x3 + 7, y1 + 10, "_");
		print_text(x3 + 8, y1 + 10, "_");
		print_text(x3 + 9, y1 + 10, "_");

		// G - outer left side
		print_text(x3 + 1, y1 + 1, "/");
		print_text(x3, y1 + 2, "/");
		print_text(x3, y1 + 3, "|");
		print_text(x3, y1 + 4, "|");
		print_text(x3, y1 + 5, "|");
		print_text(x3, y1 + 6, "|");
		print_text(x3, y1 + 7, "|");
		print_text(x3, y1 + 8, "|");
		print_text(x3 + 1, y1 + 9, "\\");
		print_text(x3 + 2, y1 + 10, "\\");

		// G - inner left side
		print_text(x3 + 2, y1 + 2, "/");
		print_text(x3 + 2, y1 + 3, "|");
		print_text(x3 + 2, y1 + 4, "|");
		print_text(x3 + 2, y1 + 5, "|");
		print_text(x3 + 2, y1 + 6, "|");
		print_text(x3 + 2, y1 + 7, "|");
		print_text(x3 + 2, y1 + 8, "\\");
		print_text(x3 + 3, y1 + 9, "\\");

		// G - inner right side
		print_text(x3 + 5, y1 + 6, "|");
		print_text(x3 + 8, y1 + 7, "|");
		print_text(x3 + 8, y1 + 8, "|");
		print_text(x3 + 8, y1 + 9, "|");

		// G - outer right side
		print_text(x3 + 10, y1 + 1, "|");
		print_text(x3 + 10, y1 + 6, "|");
		print_text(x3 + 10, y1 + 7, "|");
		print_text(x3 + 10, y1 + 8, "|");
		print_text(x3 + 10, y1 + 9, "|");
		print_text(x3 + 10, y1 + 10, "/");
	}
	cout << color.get("normal");
	Sleep(1500);

	temp_buffer.loadFromFile("GameFiles\\Scenarios\\DefaultScenario\\Resources\\Sounds\\Fight\\Miss1.ogg");
	temp_sound.play();
	print_box(0, 0, 54, 17);
	Sleep(3000);
}

void game::main_menu()
{
	set_font_size(global_font_size + 32);
	unsigned int choice = 0;
	vector<string> options;
	string title;

	while (playing_)
	{
		// Displaying Menu Options ----------------
		clear_console();
		print_box(0, 0, 54, 17);
		options_menu(choice, s_line_texts["MainMenuTitle"], m_line_texts["MainMenu"], false);
		main_menu_execution(choice);
	}
	save_settings();
}

// Saving and Loading ---------------------
void game::load_menu()
{
	vector<string> used_names;
	unsigned int choice = 0;

	if (count_saves() > 0)
	{
		for (auto& p : filesystem::directory_iterator("Saves\\" + current_scenario))
		{
			string temp = p.path().filename().string();
			temp.erase(temp.size() - 5, 5);
			used_names.push_back(temp);
		}
		sort(used_names.begin(), used_names.end());
	}

	clear_console();
	print_box(0, 0, 54, 17);
	options_menu(choice, s_line_texts["LoadMenuTitle"], s_line_texts["Return"], used_names, true);

	if (choice == 0) in_session_ = false;
	else
	{
		hero_.clear();
		load(used_names[choice - 1]);
		sounds.main_menu(false);
	}
}
