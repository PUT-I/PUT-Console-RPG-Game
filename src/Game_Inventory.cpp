#include "game.hpp"

#include "color_util.hpp"
#include "console_util.hpp"
#include "enemy.hpp"
#include "global_functions.hpp"
#include "global_variables.hpp"
#include "interface_util.hpp"
#include "sound_manager.hpp"

using namespace std;

enemy none_enemy;
shared_ptr<character> none_enemy_ptr = make_shared<enemy>(none_enemy);

// Inventory ------------------------------
const bool game::inventory_menus_execution(const shared_ptr<item>& item1)
{
	bool result = true;

	if (item1->get_type() == item::weapon)
	{
		const auto weapon1 = dynamic_pointer_cast<weapon>(item1);
		if (weapon1->get_requirements() > hero_.get_strength())
		{
			result = false;
		}
		else
		{
			hero_.equip(weapon1);
		}
	}
	else if (item1->get_type() == item::armor)
	{
		const auto armor1 = dynamic_pointer_cast<armor>(item1);
		if (armor1->get_requirements() > hero_.get_strength())
		{
			result = false;
		}
		else
		{
			hero_.equip(armor1);
		}
	}
	else if (item1->get_type() == item::spell)
	{
		const auto spell1 = dynamic_pointer_cast<spell>(item1);
		if (hero_.get_mp() < spell1->get_mana())
		{
			result = false;
		}
		else if (spell1->get_effect() != "heal" && spell1->get_effect() != "stamina")
		{
			result = false;
		}
		else if (spell1->get_effect() == "heal" && hero_.get_hp() >= hero_.get_hp_max())
		{
			result = false;
		}
		else
		{
			hero_.cast_spell(*dynamic_pointer_cast<spell>(item1), none_enemy_ptr);
			hero_.active_spells_refresh();
			hero_.clear_active_spells();
		}
	}
	else if (item1->get_type() == item::consumable)
	{
		const auto consumable1 = dynamic_pointer_cast<consumable>(item1);
		if (consumable1->get_effect() == "heal" && hero_.get_hp() >= hero_.get_hp_max())
		{
			result = false;
		}
		else if (consumable1->get_effect() == "stamina" && hero_.get_sp() >= hero_.get_sp_max())
		{
			result = false;
		}
		else if (consumable1->get_effect() == "mana" && hero_.get_mp() >= hero_.get_mp_max())
		{
			result = false;
		}
		else
		{
			hero_.consume(item1);
		}
	}

	return result;
}

void game::display_inv_gfx(const shared_ptr<item>& item1) const
{
	const string money_text = color.get("yellow") + s_line_texts["ShoppingYourMoney"] + to_string(hero_.get_money()) +
		color.get("normal") + "   ";

	print_text(1, 1, money_text);

	if (item1 != nullptr)
	{
		display_gfx(35, 3, graphics[item1->get_gfx_dir()]);
		if (item1->get_type() == item::weapon)
		{
			const auto weapon1 = dynamic_pointer_cast<weapon>(item1);

			print_box(34, 2, 20, 5);
			print_box(34, 8, 20, 4);

			// Displaying Parameters ------------------
			cout << color.get("yellow");
			print_text(35, 9, m_line_texts["InventoryWeapons"][0], weapon1->get_price());
			cout << color.get("normal");
			print_text(35, 10, m_line_texts["InventoryWeapons"][1], weapon1->get_dmg_min());
			print_text(35, 11, m_line_texts["InventoryWeapons"][2], weapon1->get_dmg_max());
			print_text(35, 12, m_line_texts["InventoryWeapons"][3], weapon1->get_requirements());
		}
		else if (item1->get_type() == item::armor)
		{
			const auto armor1 = dynamic_pointer_cast<armor>(item1);

			print_box(34, 2, 20, 6);
			print_box(34, 9, 20, 3);

			// Displaying Parameters ------------------
			cout << color.get("yellow");
			print_text(35, 10, m_line_texts["InventoryArmors"][0], armor1->get_price());
			cout << color.get("normal");
			print_text(35, 11, m_line_texts["InventoryArmors"][1], armor1->get_dmg_protection());
			print_text(35, 12, m_line_texts["InventoryArmors"][2], armor1->get_requirements());
		}
		else if (item1->get_type() == item::consumable)
		{
			const auto consumable1 = dynamic_pointer_cast<consumable>(item1);
			string effect;

			print_box(34, 2, 20, 5);
			print_box(34, 8, 20, 2);

			// Displaying Parameters ------------------
			cout << color.get("yellow");
			print_text(35, 9, m_line_texts["InventoryConsumables"][0], consumable1->get_price());
			cout << color.get("normal");
			{
				if (consumable1->get_effect() == "heal")
				{
					effect = color.get("red") + "+" + to_string(consumable1->get_power());
					effect += "hp" + color.get("normal");
				}
				else if (consumable1->get_effect() == "stamina")
				{
					effect = color.get("green") + "+" + to_string(consumable1->get_power());
					effect += "sp" + color.get("normal");
				}
				else if (consumable1->get_effect() == "mana")
				{
					effect = color.get("blue") + "+" + to_string(consumable1->get_power());
					effect += "mp" + color.get("normal");
				}
			}
			print_text(35, 10, m_line_texts["InventoryConsumables"][1], effect);
		}
		else if (item1->get_type() == item::spell)
		{
			const auto spell1 = dynamic_pointer_cast<spell>(item1);
			string effect;
			int cost;

			if (spell1->get_effect() == "heal")
			{
				effect = color.get("red") + "+" + to_string(spell1->get_power());
				effect += "hp" + color.get("normal");
			}
			else if (spell1->get_effect() == "stamina")
			{
				effect = color.get("green") + "+" + to_string(spell1->get_power());
				effect += "sp" + color.get("normal");
			}
			else if (spell1->get_effect() == "fire")
			{
				effect = "+" + to_string(spell1->get_power());
				effect += "dmg";
			}

			// Cost -----------------------------------
			{
				cost = int(
					spell1->get_mana() * ((100.0f + spell1->get_requirements() - hero_.get_willpower()) / 100.0f));
				if (cost > spell1->get_mana()) cost = spell1->get_mana();
				else if (cost < spell1->get_requirements()) cost = spell1->get_requirements();
			}

			print_box(34, 2, 20, 6);
			print_box(34, 9, 20, 5);
			cout << color.get("yellow");
			print_text(35, 10, m_line_texts["InventorySpells"][0], spell1->get_price());
			cout << color.get("normal");
			print_text(35, 11, m_line_texts["InventorySpells"][1], effect);
			cout << color.get("blue");
			print_text(35, 12, m_line_texts["InventorySpells"][2], cost);
			cout << color.get("normal");

			print_text(35, 13, m_line_texts["InventorySpells"][3], spell1->get_span());
			print_text(35, 14, m_line_texts["InventorySpells"][4], spell1->get_requirements());
		}
	}
}

void game::armors_menu()
{
	unsigned int choice = 0;
	bool action_exe;
	int action = 0;

	vector<shared_ptr<item>> armors = item_type_find(hero_.get_inventory(), item::armor);
	clear_console();

	const vector<string> blank1(6, string(20, ' '));
	const vector<string> blank2(3, string(20, ' '));
	const string blank(s_line_texts["MsgReq"].size(), ' ');

	print_box(34, 2, 20, 6);
	print_box(34, 8, 20, 3);
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["InventoryArmorsTitle"].size() / 2, 0, s_line_texts["InventoryArmorsTitle"]);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= armors.size() && action == 0)
			{
				display_gfx(35, 3, blank1);
				display_gfx(35, 9, blank2);

				// Requirements Message -----------------
				{
					cursor_pos(55 - s_line_texts["MsgReq"].size(), 1);
					if (choice != 0 && armors[choice - 1]->get_requirements() > hero_.get_strength())
					{
						cout << color.get("red") << s_line_texts["MsgReq"] << color.get("normal");
					}
					else
					{
						cout << blank;
					}
				}

				cursor_pos(2, 1);
				display_options_no_num(choice, s_line_texts["Return"], armors, true);
				if (choice != 0)
				{
					const auto armor1 = dynamic_pointer_cast<armor>(armors[choice - 1]);
					display_gfx(35, 3, graphics[armor1->get_gfx_dir()]);
					cout << color.get("yellow");
					print_text(35, 9, m_line_texts["InventoryArmors"][0], armor1->get_price());
					cout << color.get("normal");
					print_text(35, 10, m_line_texts["InventoryArmors"][1], armor1->get_dmg_protection());
					print_text(35, 11, m_line_texts["InventoryArmors"][2], armor1->get_requirements());
				}
			}
			choice_limit(choice, armors.size() + 1);

			action = up_down_esc_controls(choice, armors.size() + 1);
		}
		while (action == 0);

		if (action == -1)
		{
			choice = 0;
		}

		if (choice == 0) return;

		action_exe = inventory_menus_execution(armors[choice - 1]);
	}
	while (!action_exe);
}

void game::weapons_menu()
{
	unsigned int choice = 0;
	bool action_exe;
	int action = 0;

	vector<shared_ptr<item>> weapons = item_type_find(hero_.get_inventory(), item::weapon);

	clear_console();

	const vector<string> blank1(5, string(20, ' '));
	const vector<string> blank2(4, string(20, ' '));
	const string blank(s_line_texts["MsgReq"].size(), ' ');

	print_box(34, 2, 20, 5);
	print_box(34, 8, 20, 4);
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["InventoryWeaponsTitle"].size() / 2, 0, s_line_texts["InventoryWeaponsTitle"]);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= weapons.size() && action == 0)
			{
				display_gfx(35, 3, blank1);
				display_gfx(35, 9, blank2);

				// Requirements Message -----------------
				{
					cursor_pos(55 - s_line_texts["MsgReq"].size(), 1);
					if (choice != 0 && weapons[choice - 1]->get_requirements() > hero_.get_strength())
					{
						cout << color.get("red") << s_line_texts["MsgReq"] << color.get("normal");
					}
					else print_text(55 - blank.size(), 1, blank);
				}

				cursor_pos(2, 1);
				display_options_no_num(choice, s_line_texts["Return"], weapons, true);
				if (choice != 0)
				{
					const auto weapon1 = dynamic_pointer_cast<weapon>(weapons[choice - 1]);
					display_gfx(35, 3, graphics[weapon1->get_gfx_dir()]);
					cout << color.get("yellow");
					print_text(35, 9, m_line_texts["InventoryWeapons"][0], weapon1->get_price());
					cout << color.get("normal");
					print_text(35, 10, m_line_texts["InventoryWeapons"][1], weapon1->get_dmg_min());
					print_text(35, 11, m_line_texts["InventoryWeapons"][2], weapon1->get_dmg_max());
					print_text(35, 12, m_line_texts["InventoryWeapons"][3], weapon1->get_requirements());
				}
			}
			choice_limit(choice, weapons.size() + 1);

			action = up_down_esc_controls(choice, weapons.size() + 1);
		}
		while (action == 0);

		if (action == -1)
		{
			choice = 0;
		}

		if (choice == 0) return;

		action_exe = inventory_menus_execution(weapons[choice - 1]);
	}
	while (!action_exe);
}

void game::consumables_menu()
{
	unsigned int choice = 0;
	bool action_exe;
	int action = 0;

	vector<shared_ptr<item>> consumables = item_type_find(hero_.get_inventory(), item::consumable);
	clear_console();

	const vector<string> blank1(5, string(20, ' '));
	const vector<string> blank2(2, string(20, ' '));

	unsigned int greatest_size;
	if (s_line_texts["MsgHpFull"].size() > s_line_texts["MsgSpFull"].size())
		greatest_size = s_line_texts["MsgHpFull"].
			size();
	else greatest_size = s_line_texts["MsgSpFull"].size();
	if (s_line_texts["MsgMpFull"].size() > greatest_size) greatest_size = s_line_texts["MsgMpFull"].size();
	const string blank(greatest_size, ' ');

	print_box(34, 2, 20, 5);
	print_box(34, 8, 20, 2);
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["InventoryConsumables"].size() / 2, 0, s_line_texts["InventoryConsumables"]);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= consumables.size() && action == 0)
			{
				const auto consumable1 = dynamic_pointer_cast<consumable>(consumables[choice - 1]);
				display_gfx(35, 3, blank1);
				display_gfx(35, 9, blank2);

				// Stat Is Full Message -------------------
				{
					print_text(55 - blank.size(), 1, blank);
					if (choice != 0 && consumable1->get_effect() == "heal" && hero_.get_hp() >= hero_.
						get_hp_max())
					{
						cursor_pos(55 - s_line_texts["MsgHpFull"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgHpFull"] << color.get("normal");
					}
					else if (choice != 0 && consumable1->get_effect() == "stamina" && hero_.get_sp() >= hero_.
						get_sp_max())
					{
						cursor_pos(55 - s_line_texts["MsgSpFull"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgSpFull"] << color.get("normal");
					}
					else if (choice != 0 && consumable1->get_effect() == "mana" && hero_.get_mp() >= hero_.
						get_mp_max())
					{
						cursor_pos(55 - s_line_texts["MsgMpFull"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgMpFull"] << color.get("normal");
					}
				}

				cursor_pos(2, 1);
				display_options_no_num(choice, s_line_texts["Return"], consumables, true);
				if (choice != 0)
				{
					display_gfx(35, 3, graphics[consumables[choice - 1]->get_gfx_dir()]);
					cout << color.get("yellow");
					print_text(35, 9, m_line_texts["InventoryConsumables"][0], consumable1->get_price());
					cout << color.get("normal");

					// Displaying Parameters ------------------
					{
						string effect;
						if (consumable1->get_effect() == "heal")
						{
							effect = color.get("red") + "+" + to_string(consumable1->get_power());
							effect += "hp" + color.get("normal");
						}
						else if (consumable1->get_effect() == "stamina")
						{
							effect = color.get("green") + "+" + to_string(consumable1->get_power());
							effect += "sp" + color.get("normal");
						}
						else if (consumable1->get_effect() == "mana")
						{
							effect = color.get("blue") + "+" + to_string(consumable1->get_power());
							effect += "mp" + color.get("normal");
						}
						print_text(35, 10, m_line_texts["InventoryConsumables"][1], effect);
					}
				}
			}
			choice_limit(choice, consumables.size() + 1);

			action = up_down_esc_controls(choice, consumables.size() + 1);
		}
		while (action == 0);

		if (action == -1)
		{
			choice = 0;
		}
		if (choice == 0) return;

		action_exe = inventory_menus_execution(consumables[choice - 1]);
	}
	while (!action_exe);
}

const bool game::spells_menu(enemy& enemy)
{
	unsigned int choice = 0;
	int cost = NULL;
	bool action_exe;
	int action = 0;

	vector<shared_ptr<item>> spells = item_type_find(hero_.get_inventory(), item::spell);
	clear_console();

	const vector<string> blank1(6, string(20, ' '));
	const vector<string> blank2(4, string(20, ' '));

	unsigned int greatest_size;
	if (s_line_texts["MsgHpFull"].size() > s_line_texts["MsgSpFull"].size())
		greatest_size = s_line_texts["MsgHpFull"].
			size();
	else greatest_size = s_line_texts["MsgSpFull"].size();
	if (s_line_texts["MsgMpFull"].size() > greatest_size) greatest_size = s_line_texts["MsgMpFull"].size();
	const string blank(greatest_size, ' ');

	print_box(34, 2, 20, 6);
	print_box(34, 9, 20, 4);
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["InventorySpellsTitle"].size() / 2, 0, s_line_texts["InventorySpellsTitle"]);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= spells.size() && action == 0)
			{
				const auto spell1 = dynamic_pointer_cast<spell>(spells[choice - 1]);
				display_gfx(35, 3, blank1);
				display_gfx(35, 10, blank2);

				// Stat Is Full or Empty Message ----------
				{
					print_text(55 - blank.size(), 1, blank);
					if (choice != 0 && spell1->get_effect() == "heal" && hero_.get_hp() >= hero_.get_hp_max())
					{
						cursor_pos(55 - s_line_texts["MsgHpFull"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgHpFull"] << color.get("normal");
					}
					else if (choice != 0 && spell1->get_effect() == "stamina" && hero_.get_sp() >= hero_.
						get_sp_max())
					{
						cursor_pos(55 - s_line_texts["MsgSpFull"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgSpFull"] << color.get("normal");
					}
					else if (choice != 0 && hero_.get_mp() <= 0 && (spell1->get_effect() == "heal" || enemy.
						get_name() != "NONE"))
					{
						cursor_pos(55 - s_line_texts["MsgMpEmpty"].size(), 1);
						cout << color.get("red") << s_line_texts["MsgMpEmpty"] << color.get("normal");
					}
				}

				cursor_pos(2, 1);
				display_options_no_num(choice, s_line_texts["Return"], spells, false);
				if (choice != 0)
				{
					display_gfx(35, 3, graphics[spell1->get_gfx_dir()]);

					// Displaying Parameters ------------------
					{
						string effect;
						if (spell1->get_effect() == "heal")
						{
							effect = color.get("red") + "+" + to_string(spell1->get_power());
							effect += "hp" + color.get("normal");
						}
						else if (spell1->get_effect() == "stamina")
						{
							effect = color.get("green") + "+" + to_string(spell1->get_power());
							effect += "sp" + color.get("normal");
						}
						else if (spell1->get_effect() == "fire")
						{
							effect = "+" + to_string(spell1->get_power());
							effect += "dmg";
						}
						print_text(35, 10, m_line_texts["InventorySpells"][1], effect);

						// Cost -----------------------------------
						{
							cost = int(
								spell1->get_mana() * ((100.0f + spell1->get_requirements() - hero_.
									get_willpower()) / 100.0f));
							if (cost > spell1->get_mana()) cost = spell1->get_mana();
							else if (cost < spell1->get_requirements())
								cost = spell1->
									get_requirements();
						}

						cout << color.get("blue");
						print_text(35, 11, m_line_texts["InventorySpells"][2], cost);
						cout << color.get("normal");
						print_text(35, 12, m_line_texts["InventorySpells"][3], spell1->get_span());
						print_text(35, 13, m_line_texts["InventorySpells"][4], spell1->get_requirements());
					}
				}
			}
			choice_limit(choice, spells.size() + 1);
			action = up_down_esc_controls(choice, spells.size() + 1);
		}
		while (action == 0);

		if (action == -1)
		{
			choice = 0;
		}

		if (choice == 0) return false;

		if (enemy.get_name() == "NONE")
		{
			action_exe = inventory_menus_execution(spells[choice - 1]);
		}
		else
		{
			action_exe = spells_execution(choice, dynamic_pointer_cast<spell>(spells[choice - 1]), enemy);
		}
	}
	while (!action_exe);

	return true;
}

const bool game::inventory_menu_execution(unsigned int& choice)
{
	switch (choice)
	{
	case 0:
		{
			return true;
		}
	case 1:
		{
			weapons_menu();
			break;
		}
	case 2:
		{
			armors_menu();
			break;
		}
	case 3:
		{
			consumables_menu();
			break;
		}
	case 4:
		{
			spells_menu(none_enemy);
			break;
		}

	default:
		return true;
	}

	choice = 0;
	return false;
}

void game::inventory_menu()
{
	unsigned int choice = 0;
	int action;

	// Looping Until Return in Execution ------
	do
	{
		clear_console();
		print_box(0, 0, 54, 17);
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice < m_line_texts["Inventory"].size())
			{
				display_options_menu(choice, s_line_texts["InventoryTitle"], m_line_texts["Inventory"]);
			}
			choice_limit(choice, m_line_texts["Inventory"].size());
			action = up_down_esc_controls(choice, m_line_texts["Inventory"].size());
		}
		while (action == 0);
		hero_.remove_from_inventory();
		if (action == -1)
		{
			choice = 0;
		}
	}
	while (!inventory_menu_execution(choice));
}

void game::shopping_buy_menu(vector<shared_ptr<item>> items)
{
	unsigned int choice = 0;
	int action = 0;

	const vector<string> blank(15, string(21, ' '));

	unsigned int greatest_size;
	if (s_line_texts["MsgHaveSpell"].size() > s_line_texts["MsgMoney"].size())
		greatest_size = s_line_texts["MsgMoney"].
			size();
	else greatest_size = s_line_texts["MsgMoney"].size();
	const string blank_line(greatest_size, ' ');

	// Removing Spells Hero Already Has -------
	{
		for (const shared_ptr<item>& item : hero_.get_inventory())
		{
			for (auto it = items.begin(); it != items.end(); ++it)
			{
				if ((*it)->get_type() == item::spell && (*it)->get_name() == item->get_name())
				{
					items.erase(it);
					break;
				}
			}
		}
	}

	clear_console();
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["ShoppingBuyTitle"].size() / 2, 0, s_line_texts["ShoppingBuyTitle"]);
	display_inv_gfx(nullptr);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= items.size() && action == 0)
			{
				print_text(55 - blank_line.size(), 1, blank_line);
				display_gfx(34, 2, blank);
				cursor_pos(2, 2);
				display_options_no_num(choice, s_line_texts["Return"], items, false);
			}
			if (choice > 0 && choice <= items.size())
			{
				display_inv_gfx(items[choice - 1]);

				// Displaying Messages --------------------
				print_text(55 - blank_line.size(), 1, blank_line);
				if (items[choice - 1]->get_type() == item::spell && find(hero_.get_inventory().begin(),
				                                                         hero_.get_inventory().end(),
				                                                         items[choice - 1]) !=
					hero_.get_inventory().end())
				{
					cursor_pos(55 - s_line_texts["MsgHaveSpell"].size(), 1);
					cout << color.get("red") << s_line_texts["MsgHaveSpell"] << color.get("normal");
				}
				else if (hero_.get_money() < items[choice - 1]->get_price())
				{
					cursor_pos(55 - s_line_texts["MsgMoney"].size(), 1);
					cout << color.get("red") << s_line_texts["MsgMoney"] << color.get("normal");
				}
			}
			choice_limit(choice, items.size() + 1);

			action = up_down_esc_controls(choice, items.size() + 1);
		}
		while (action == 0);

		if (action == 1 && choice > 0)
		{
			if (items[choice - 1]->get_type() == item::spell && find(hero_.get_inventory().begin(),
			                                                         hero_.get_inventory().end(), items[choice - 1]) !=
				hero_.get_inventory().end())
			{
			}
			else if (hero_.get_money() >= items[choice - 1]->get_price())
			{
				hero_.decrease_money(items[choice - 1]->get_price());
				hero_.add_to_inventory(items[choice - 1]);
				sounds.play_sound(sound_manager::character, "Trade1");
			}
		}
		if (action == -1)
		{
			choice = 0;
		}

		if (choice == 0) return;
	}
	while (true);
}

void game::shopping_sell_menu()
{
	unsigned int choice = 0;
	int action = 0;

	const vector<string> blank(15, string(21, ' '));

	vector<shared_ptr<item>> items = hero_.get_inventory();
	vector<shared_ptr<item>> spells;

	// Spell Removing -------------------------
	bool removed;
	do
	{
		removed = false;
		for (auto item = items.begin(); item != items.end(); ++item)
		{
			if ((*item)->get_type() == item::spell)
			{
				spells.push_back(*item);
				items.erase(item);
				removed = true;
				break;
			}
		}
	}
	while (removed);

	clear_console();
	print_box(0, 0, 54, 17);
	print_text(28 - s_line_texts["ShoppingSellTitle"].size() / 2, 0, s_line_texts["ShoppingSellTitle"]);
	display_inv_gfx(nullptr);

	do
	{
		// Displaying Until Enter -----------------
		do
		{
			if (choice >= 0 && choice <= items.size() && action == 0)
			{
				display_gfx(34, 1, blank);
				cursor_pos(2, 2);
				display_options_no_num(choice, s_line_texts["Return"], items, true);
			}
			if (choice > 0 && choice <= items.size()) display_inv_gfx(items[choice - 1]);
			choice_limit(choice, items.size() + 1);

			action = up_down_esc_controls(choice, items.size() + 1);
		}
		while (action == 0);
		if (action == 1 && choice > 0 && items[choice - 1]->get_type() != item::spell)
		{
			hero_.add_money(items[choice - 1]->get_price());
			items[choice - 1]->decrease_number();
			action = 0;
			sounds.play_sound(sound_manager::character, "Trade1");
			if (items[choice - 1]->get_number() == 0)
			{
				items.erase(items.begin() + choice - 1);
				choice = 0;
				clear_console();
				print_box(0, 0, 54, 17);
				print_text(28 - s_line_texts["ShoppingBuyTitle"].size() / 2, 0, s_line_texts["ShoppingBuyTitle"]);
				display_inv_gfx(nullptr);
				continue;
			}
		}
		if (action == -1)
		{
			choice = 0;
		}

		if (choice == 0)
		{
			// Spell Re-adding ------------------------
			for (const shared_ptr<item>& spell : spells)
			{
				items.push_back(spell);
			}
			hero_.replace_inventory(items);

			return;
		}
	}
	while (true);
}

void game::shopping_choice_menu(const vector<shared_ptr<item>>& items)
{
	unsigned int choice = 0;
	int action = 0;

	clear_console();
	set_font_size(global_font_size + 32);

	do
	{
		// Displaying Until Enter Or Esc-----------
		print_box(0, 0, 54, 17);
		do
		{
			if (choice >= 0 && choice <= 2 && action == 0)
				display_options_menu(
					choice, s_line_texts["ShoppingTitle"], m_line_texts["Shopping"]);
			choice_limit(choice, 3);

			action = up_down_esc_controls(choice, 3);
		}
		while (action == 0);

		if (action == 1 && choice != 0)
		{
			const int choice_temp = choice;

			if (choice == 1) shopping_buy_menu(items);
			else if (choice == 2) shopping_sell_menu();

			choice = choice_temp;
			action = 0;
			clear_console();
		}

		if (action == -1)
		{
			choice = 0;
		}
		if (choice == 0) break;
	}
	while (true);

	clear_console();
	set_font_size(global_font_size + story_font_inc);
}
