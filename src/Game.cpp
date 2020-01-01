#include "game.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "color_util.hpp"
#include "console_util.hpp"
#include "enemy.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"
#include "interface_util.hpp"
#include "sound_manager.hpp"

using namespace std;
using namespace nlohmann;

game::game()
{
	// Settings loading -----------------------
	if (!files::exists("GameFiles\\settings.json"))
	{
		adapt_font_size();
		language = "en";
		code_page = "1250";
		current_scenario = "defaultScenario";
		save_settings();
	}
	else
	{
		load_settings();
	}
	adapt_location_font_size();
	load_scenario();

	playing_ = true;
	in_session_ = false;

	full_screen(true);
	refresh_code_page();
	show_console_cursor(false);
	logo();
}

/*------------------------------ Functions ------------------------------*/
// Safeguards and initialization ----------
const bool game::name_used(const string& name)
{
	string temp;

	// Checking if Used -----------------------
	for (auto& p : filesystem::directory_iterator("Saves\\" + current_scenario))
	{
		if (name + ".json" == p.path().filename().string())
		{
			return true;
		}
	}
	return false;
}

const unsigned int game::count_saves() const
{
	unsigned int counter = 0;
	if (filesystem::is_empty("Saves\\" + current_scenario) != true)
	{
		for (auto& dummy : filesystem::directory_iterator("Saves\\" + current_scenario))
		{
			(void)dummy;
			counter++;
		}
	}
	return counter;
}

void game::load_resource_maps()
{
	// Object Maps loading --------------------
	{
		items::weapons.clear();
		items::armors.clear();
		items::spells.clear();
		items::consumables.clear();
		enemies.clear();

		json items;
		std::ifstream in("GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\items.json");
		in >> items;
		in.close();

		files::load_map(items["weapons"], items::weapons);
		files::load_map(items["armors"], items::armors);
		files::load_map(items["spells"], items::spells);
		files::load_map(items["consumables"], items::consumables);

		// Enemies loading ------------------------
		json enemy_tab;
		enemy enemy_temp;

		in.open("GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\enemies.json");
		in >> enemy_tab;
		in.close();
		for (const json& e : enemy_tab["tab"])
		{
			enemy_temp.load(e["enemyInfo"]);
			enemies.insert(std::pair(e["key"], enemy_temp));
		}
	}
}

void game::initialize()
{
	string name;

	do
	{
		// Name Setting ---------------------------
		clear_console();
		print_box(0, 0, 54, 17);
		print_text(3, 2, s_line_texts["Initialize1"]);

		input_string(name, 15); // Input with letter limit

		if (name_used(name))
		{
			print_text(3, 4, s_line_texts["Initialize2"]);
			cursor_pos(27 + s_line_texts["Initialize2"].size() / 6, 5);
			space_pause();
			clear_console();
			continue;
		}

		clear_console();
		print_box(0, 0, 54, 17);
		cursor_pos(3, 2);
	}
	while (name_used(name) || !confirmation_menu(s_line_texts["Initialize3-1"] + name + s_line_texts["Initialize3-2"]));

	// Hero Initializing ---------------------
	this->hero_.initialize(name);

	hero_.add_to_inventory(items::spells["Fire I"]);
	hero_.add_to_inventory(items::spells["Heal I"]);

	in_session_ = true;
}

// Saving and Loading ---------------------
void game::load(const string& heroName)
{
	hero_.load(heroName);
}

void game::delete_saves_menu() const
{
	do
	{
		vector<string> used_names;
		string temp;
		unsigned int choice = 0;
		if (count_saves() > 0)
		{
			for (auto& p : filesystem::directory_iterator("Saves\\" + current_scenario))
			{
				temp = p.path().filename().string();
				temp.erase(temp.size() - 5, 5);
				used_names.push_back(temp);
			}
			sort(used_names.begin(), used_names.end());
		}
		clear_console();
		print_box(0, 0, 54, 17);
		options_menu(choice, s_line_texts["DeleteSavesMenuTitle"], s_line_texts["Return"], used_names, true);

		if (choice == 0) return;
		else
		{
			clear_console();
			print_box(0, 0, 54, 17);
			cursor_pos(3, 2);
			if (confirmation_menu(s_line_texts["MainMenuQuestion"]))
			{
				filesystem::remove("Saves\\" + used_names[choice - 1] + ".json");
			}
		}
	}
	while (true);
}

void game::hero_to_temp_hero()
{
	temp_hero_ = hero_;
	vector<shared_ptr<item>> temp_inv;

	/*
	This part is needed in order to prevent reducing number
	of items after copying hero to tempHero, which may
	occur beacuse of the smart pointer usage
	*/
	for (shared_ptr<item>& e : hero_.get_inventory())
	{
		if (e->get_type() == item::weapon)
		{
			temp_inv.push_back(make_shared<weapon>(*dynamic_pointer_cast<weapon>(e)));
		}
		else if (e->get_type() == item::armor)
		{
			temp_inv.push_back(make_shared<armor>(*dynamic_pointer_cast<armor>(e)));
		}
		else if (e->get_type() == item::consumable)
		{
			temp_inv.push_back(make_shared<consumable>(*dynamic_pointer_cast<consumable>(e)));
		}
		else if (e->get_type() == item::spell)
		{
			temp_inv.push_back(make_shared<spell>(*dynamic_pointer_cast<spell>(e)));
		}
	}
}

// Battle System --------------------------
const bool game::spells_execution(const unsigned int& choice, const shared_ptr<spell>& spell1, enemy& enemy1)
{
	bool result;
	const shared_ptr<character> enemy_ptr = make_shared<enemy>(enemy1);

	cursor_pos_begin();

	if (hero_.get_mp() < spell1->get_mana())
	{
		result = false;
	}
	else if (spell1->get_effect() == "heal" && hero_.get_hp() >= hero_.get_hp_max())
	{
		result = false;
	}
	else if (spell1->get_effect() == "stamina" && hero_.get_sp() >= hero_.get_sp_max())
	{
		result = false;
	}
	else
	{
		hero_.cast_spell(*dynamic_pointer_cast<spell>(spell1), enemy_ptr);
		result = true;
	}

	enemy1 = *dynamic_pointer_cast<enemy>(enemy_ptr);

	return result;
}

void game::display_fight_stats(const enemy& enemy1) const
{
	// Stats Hero ----------------------------
	{
		cursor_pos(1, 1);
		cout << hero_.get_name();
		cursor_pos(1, 2);
		cout << color.get("red") << m_line_texts["FightStats"][0] << hero_.get_hp() << "/" << hero_.get_hp_max() <<
			color.
			get("normal") << "  ";
		cursor_pos(1, 3);
		cout << color.get("green") << m_line_texts["FightStats"][1] << hero_.get_sp() << "/" << hero_.get_sp_max() <<
			color.get("normal") << "  ";
		cursor_pos(1, 4);
		cout << color.get("blue") << m_line_texts["FightStats"][2] << hero_.get_mp() << "/" << hero_.get_mp_max() <<
			color
			.get("normal") << "  ";
		cursor_pos(1, 5);
		cout << m_line_texts["FightStats"][3] << hero_.get_defence() << "  ";
		cursor_pos(1, 6);
		cout << m_line_texts["FightStats"][4] << hero_.get_evasion_c() << "  ";
		cursor_pos(1, 7);
		for (int i = 1; i <= 22; i++)
		{
			cout << "=";
		}
	}
	// Stats Enemy ----------------------------
	{
		cursor_pos(1, 8);
		cout << enemy1.load_name();
		cursor_pos(1, 9);
		cout << color.get("red") << m_line_texts["FightStats"][0] << enemy1.get_hp() << "/" << enemy1.get_hp_max() <<
			color.get("normal") << "  ";
		cursor_pos(1, 10);
		cout << m_line_texts["FightStats"][3] << enemy1.get_defence() << "  ";
		cursor_pos(1, 11);
		cout << m_line_texts["FightStats"][4] << enemy1.get_evasion_c() << "  ";
	}
}

void game::fight_execution(enemy& enemy1)
{
	enemy1.refresh_stat_dependent();

	const int choice = rand_int(0, 1);

	switch (choice)
	{
	case 0:
		{
			int chance = enemy1.get_hit_c() - hero_.get_evasion_c();
			if (chance < 1) chance = 1;

			if (rand_int(1, 100) >= chance)
			{
				int damage = rand_int(enemy1.get_dmg_min(), enemy1.get_dmg_max());

				if (damage < enemy1.get_dmg_min())
				{
					damage = enemy1.get_dmg_min();
				}

				if (damage - hero_.get_defence() > 0)
				{
					hero_.get_damaged(damage);
					sounds.play_sound(sound_manager::creature_hit, enemy1.get_sfx_dir());
				}
				else sounds.play_sound(sound_manager::miss);
			}
			else
			{
				sounds.play_sound(sound_manager::miss);
			}
			break;
		}

	case 1:
		{
			enemy1.increase_def(rand_int(1, hero_.get_dmg_min() - enemy1.get_defence()));
			sounds.play_sound(sound_manager::defend);
			break;
		}
	default: break;
	}
}

const bool game::fight_execution(const unsigned int& choice, enemy& enemy1)
{
	hero_.refresh_stat_dependent();
	hero_.refresh_item_dependent();

	switch (choice)
	{
	case 0:
		{
			int chance = hero_.get_hit_c() - enemy1.get_evasion_c();
			if (chance < 1) chance = 1;

			if (rand_int(1, 100) <= chance)
			{
				int damage = rand_int((hero_.get_sp() / hero_.get_sp_max()) * hero_.get_dmg_max(), hero_.get_dmg_max());

				if (damage < hero_.get_dmg_min())
				{
					damage = hero_.get_dmg_min();
				}

				if (damage - enemy1.get_defence() > 0)
				{
					enemy1.get_damaged(damage);
					sounds.play_sound(sound_manager::weapon, hero_.get_weapon().get_sfx_dir());
				}
				else
				{
					sounds.play_sound(sound_manager::miss);
				}
			}
			else
			{
				sounds.play_sound(sound_manager::miss);
			}

			int dec = hero_.get_weapon().get_requirements() * 10 - 2 * hero_.get_agility();
			if (dec < 1)
			{
				dec = 1;
			}

			hero_.decrease_sp(dec);
			break;
		}

	case 1:
		{
			hero_.increase_def(rand_int(1, enemy1.get_dmg_min() - hero_.get_defence()));

			int dec = hero_.get_armor().get_requirements() * 5 - 2 * hero_.get_agility();
			if (dec < 1)
			{
				dec = 1;
			}

			hero_.decrease_sp(dec);
			sounds.play_sound(sound_manager::defend);
			break;
		}

	case 2:
		{
			hero_.increase_ev(rand_int(1, 2 * hero_.get_agility()));

			int dec = hero_.get_armor().get_requirements() * 10 - 2 * hero_.get_agility();
			if (dec < 1)
			{
				dec = 1;
			}

			hero_.decrease_sp(dec);
			sounds.play_sound(sound_manager::evasion);
			break;
		}

	case 3:
		{
			clear_console();

			const bool temp = spells_menu(enemy1);

			clear_console();

			display_gfx(25, 1, graphics[enemy1.get_gfx_dir()]);
			display_options_no_num(choice, m_line_texts["Fight"], 2, 13);
			print_box(0, 0, 54, 17);
			print_box(0, 0, 22, 11);
			print_box(0, 12, 22, 5);

			if (!temp)
			{
				return false;
			}

			break;
		}

	case 4:
		{
			clear_console();
			consumables_menu();
			clear_console();

			display_gfx(25, 1, graphics[enemy1.get_gfx_dir()]);
			print_box(0, 0, 54, 17);
			print_box(0, 0, 22, 11);
			print_box(0, 12, 22, 5);

			return false;
		}
	default: break;
	}

	return true;
}

void game::fight(enemy enemy1)
{
	unsigned int choice = 0;

	clear_console();
	set_font_size(global_font_size + 32);

	display_gfx(25, 1, graphics[enemy1.get_gfx_dir()]);

	print_box(0, 0, 54, 17);
	print_box(0, 0, 22, 11);
	print_box(0, 12, 22, 5);

	do
	{
		do
		{
			display_fight_stats(enemy1);
			do
			{
				choice_limit(choice, 5);
				if (choice >= 0 && choice <= 5)
				{
					display_options_no_num(choice, m_line_texts["Fight"], 2, 13);
				}
			}
			while (!up_down_controls(choice, 5));
		}
		while (!fight_execution(choice, enemy1));

		enemy1.active_spells_refresh();
		hero_.active_spells_refresh();

		display_fight_stats(enemy1);

		if (enemy1.get_hp() <= 0) break;

		Sleep(500);
		fight_execution(enemy1);

		hero_.increase_sp(int{hero_.get_sp_max() / 100});
	}
	while (hero_.get_hp() > 0 && enemy1.get_hp() > 0);

	enemy1.clear_active_spells();
	hero_.clear_active_spells();

	if (hero_.get_hp() == 0)
	{
		clear_console();
		print_box(0, 0, 54, 17);
		cursor_pos(8, 4);
		cout << s_line_texts["MsgDead"];
		sounds.play_creature_sound("Character", "death");
		cursor_pos(8, 6);
		space_pause();
		clear_console();
		in_session_ = false;
	}
	else
	{
		clear_console();
		print_box(0, 0, 54, 17);
		cursor_pos(8, 4);
		cout << s_line_texts["MsgWin"];
		cursor_pos(8, 6);
		space_pause();
		clear_console();
		print_box(0, 0, 54, 17);
		hero_.add_exp(enemy1.get_exp());
	}

	set_font_size(global_font_size + story_font_inc);
}

// Story ----------------------------------
void game::print_menu_gfx(const vector<string>& description, const vector<string>& gfx) const
{
	display_gfx(31, 1, description);
	display_gfx(1, 1, vector<string>(20, string(29, ' ')));
	display_gfx(1, 1, gfx);
	cout << color.get("normal");
	display_gfx(30, 1, lines);
	print_box(0, 0, 81, 26);
	print_text(1, y1_ - 1, equal_signs);
};

void game::location_menu(unsigned int& choice, const vector<string>& description, const vector<string>& options,
                         const vector<string>& gfx)
{
	int temp_controls_result;
	print_menu_gfx(description, gfx);

	do
	{
		cursor_pos(0, y1_);
		display_options_menu(choice, options);
		choice_limit(choice, options.size());
		temp_controls_result = up_down_esc_controls(choice, options.size());
		if (temp_controls_result == -1)
		{
			sounds.ambience(false);
			pause_menu();
			if (!in_session_)
			{
				return;
			}
			sounds.ambience(true);
			print_menu_gfx(description, gfx);
		}
	}
	while (temp_controls_result != 1);
}

void game::location_action_gfx(const vector<string>& description) const
{
	display_gfx(31, 1, location_text_blank);
	display_gfx(1, 22, location_options_blank);
	display_gfx(31, 1, description);

	cursor_pos(1, 22);
	space_pause();
	display_gfx(31, 1, location_text_blank);
	display_gfx(1, 22, location_options_blank);
}

void game::json_to_story()
{
	clear_console();
	set_font_size(global_font_size + story_font_inc);

	unsigned int choice;
	vector<string> action, description, options;
	auto parse_action = [this, &choice, &action, &description, &options]()
	{
		for (unsigned int i = 0; i < action.size() && in_session_; i++)
		{
			if (action[i] == "sleep")
			{
				i++;
				Sleep(stoi(action[i]));
			}
			else if (action[i] == "break") { return true; }
			else if (action[i] == "clear") { clear_console(); }
			else if (action[i] == "sessionEnd") { in_session_ = false; }
			else if (action[i] == "pause")
			{
				unsigned int x_pos, y_pos;
				i++;
				x_pos = stoi(action[i]);
				i++;
				y_pos = stoi(action[i]);
				cursor_pos(x_pos, y_pos);
				space_pause();
			}
			else if (action[i] == "frame")
			{
				i++;
				if (action[i] == "small")
				{
					set_font_size(global_font_size + 32);
					print_box(0, 0, 54, 17);
				}
				else if (action[i] == "big")
				{
					set_font_size(global_font_size + story_font_inc);
					print_box(0, 0, 81, 26);
				}
			}
			else if (action[i] == "text")
			{
				unsigned int x_pos, y_pos;
				i++;
				x_pos = stoi(action[i]);
				i++;
				y_pos = stoi(action[i]);
				i++;
				print_text(x_pos, y_pos, s_line_texts[action[i]]);
			}
			else if (action[i] == "plotSwitch")
			{
				i++;
				const unsigned int index = stoi(action[i]);
				i++;
				if (action[i] == "true")
				{
					this->plot_switches_[index] = true;
				}
				else if (action[i] == "false") { this->plot_switches_[index] = false; }
			}
			else if (action[i] == "audio")
			{
				i++;
				if (action[i] == "creature")
				{
					sounds.play_creature_sound(action[i + 1], action[i + 2]);
					i += 2;
				}
				else if (action[i] == "character")
				{
					i++;
					sounds.play_sound(sound_manager::character, action[i]);
				}
				else if (action[i] == "environment")
				{
					i++;
					sounds.play_sound(sound_manager::environment, action[i]);
				}
				else if (action[i] == "misc")
				{
					i++;
					sounds.play_sound(sound_manager::misc, action[i]);
				}
				else if (action[i] == "ambience")
				{
					bool on_off;
					if (action[i + 2] == "true") { on_off = true; }
					else { on_off = false; }
					sounds.ambience(action[i + 1], on_off);
					i += 2;
				}
			}
			else if (action[i] == "choice")
			{
				i++;
				if (action[i] == "--") { choice--; }
				else if (action[i] == "++") { choice++; }
			}
			else if (action[i] == "add")
			{
				i++;
				if (action[i] == "item")
				{
					i++;
					if (action[i] == "weapon")
					{
						i++;
						hero_.add_to_inventory(items::weapons[action[i]]);
					}
					else if (action[i] == "armor")
					{
						i++;
						hero_.add_to_inventory(items::armors[action[i]]);
					}
					else if (action[i] == "spell")
					{
						i++;
						hero_.add_to_inventory(items::spells[action[i]]);
					}
					else if (action[i] == "consumable")
					{
						hero_.add_to_inventory(items::consumables[action[i + 1]], stoi(action[i + 2]));
						i += 2;
					}
				}
				else if (action[i] == "hp")
				{
					i++;
					hero_.increase_hp(stoi(action[i]));
				}
				else if (action[i] == "sp")
				{
					i++;
					hero_.increase_sp(stoi(action[i]));
				}
				else if (action[i] == "mp")
				{
					i++;
					hero_.increase_mp(stoi(action[i]));
				}
				else if (action[i] == "exp")
				{
					i++;
					hero_.add_exp(stoi(action[i]));
				}
				else if (action[i] == "money")
				{
					i++;
					hero_.add_money(stoi(action[i]));
				}
			}
			else if (action[i] == "setSubLocation")
			{
				i++;
				hero_.set_sub_location(stoi(action[i]));
			}
			else if (action[i] == "setLocation")
			{
				i++;
				hero_.set_location(stoi(action[i]));
			}
			else if (action[i] == "fight")
			{
				i++;
				fight(enemies[action[i]]);
			}
			else if (action[i] == "printMenuGfx") { i++, print_menu_gfx(vector<string>{""}, graphics[action[i]]); }
			else if (action[i] == "actionText")
			{
				i++;
				location_action_gfx(m_line_texts[action[i]]);
			}
			else if (action[i] == "description")
			{
				i++;
				description = m_line_texts[action[i]];
			}
			else if (action[i] == "optionsPopBack")
			{
				options.pop_back();
				choice--;
			}
			else if (action[i] == "merchant")
			{
				i++;
				shopping_choice_menu(files::load_inventory(
					"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\merchants.json", action[i]));
			}
			else if (action[i] == "if")
			{
				i++;
				if (action[i] == "plotSwitch")
				{
					i++;
					const unsigned int index = stoi(action[i]);
					i++;
					const unsigned int statement_true_size = stoi(action[i]);
					if (plot_switches_[index] == true)
					{
						continue;
					}
					else
					{
						i += statement_true_size + 1; // 1 is for the number of commands digit in the file
						continue;
					}
				}
			}
			else if (action[i] == "endTrue")
			{
				i++;
				i += stoi(action[i]) + 1;
			}
		}
		return false;
	};

	while (in_session_)
	{
		choice = 0;
		json sub_location = scenario_["location" + to_string(hero_.get_location())][hero_.get_sub_location() - 1];

		if (sub_location["description"] != "NONE") description = m_line_texts.at(sub_location["description"]);
		options = m_line_texts.at(sub_location["options"]);

		if (sub_location["save"] == true)hero_to_temp_hero();
		display_gfx(31, 1, location_text_blank);
		display_gfx(1, 22, location_options_blank);

		action = sub_location["preOptions"].get<vector<string>>();
		parse_action();

		while (true)
		{
			if (sub_location["ambience"][0].get<string>() != "NONE")
			{
				sounds.ambience(100.0f);
				sounds.ambience(sub_location["ambience"][0].get<string>(), sub_location["ambience"][1].get<bool>());
			}
			else if (sub_location["ambience"][0].get<string>() == "NONE")
			{
				sounds.ambience(0.0f);
				sounds.ambience(sub_location["ambience"][1].get<bool>());
			}

			location_menu(choice, description, options, graphics.at(sub_location["graphics"]));
			if (!in_session_)
			{
				break;
			}

			action = sub_location["preChoice"].get<vector<string>>();
			parse_action();

			action = sub_location["choice"][choice]["action"].get<vector<string>>();
			if (parse_action() || !in_session_) { break; }
		}
		action = sub_location["afterChoice"].get<vector<string>>();
		parse_action();
	}

	sounds.ambience(false);
	sounds.main_menu(true);
	sounds.clear_sound_list();
	set_font_size(global_font_size + 32);
}
