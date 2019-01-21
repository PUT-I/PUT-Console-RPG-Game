#include "Game.hpp"
#include "CManip.hpp"
#include "Sounds.hpp"

using namespace std;

Enemy noneEnemy;
shared_ptr<Character> noneEnemyPtr = make_shared<Enemy>(noneEnemy);

//Inventory ------------------------------
const bool Game::inventoryMenusExecution(const shared_ptr<Item> &item) {
	bool result;

	if (hero.getMp() < item->getMana()) {
		result = false;
	}
	else if (item->getRequirements() > hero.getStrength() && (item->getType() == "weapon" || item->getType() == "armor")) {
		result = false;
	}
	else if (item->getType() == "spell" && (item->getEffect() != "heal" && item->getEffect() != "stamina")) {
		result = false;
	}
	else if (item->getEffect() == "heal" && hero.getHp() >= hero.getHpMax()) {
		result = false;
	}
	else if (item->getEffect() == "stamina" && hero.getSp() >= hero.getSpMax()) {
		result = false;
	}
	else if (item->getEffect() == "mana" && hero.getMp() >= hero.getMpMax()) {
		result = false;
	}
	else {
		if (item->getType() == "weapon" || item->getType() == "armor") hero.equip(item);
		else if (item->getType() == "consumable") hero.consume(item);
		else if (item->getType() == "spell") {
			hero.castSpell(*dynamic_pointer_cast<Spell>(item), noneEnemyPtr);
			hero.activeSpellsRefresh();
			hero.clearActiveSpells();
		}
		result = true;
	}

	return result;
}

void Game::displayInvGfx(const shared_ptr<Item> &item) const
{
	PrintText(1, 1, color.get("yellow") + sLineTexts["ShoppingYourMoney"] + to_string(hero.getMoney()) + color.get("normal") + "   ");

	if (item != nullptr) {
		DisplayGfx(35, 3, graphics[item->getGfxDir()]);
		if (item->getType() == "weapon") {
			PrintBox(34, 2, 20, 5);
			PrintBox(34, 8, 20, 4);

			//Displaying Parameters ------------------
			cout << color.get("yellow");
			PrintText(35, 9, mLineTexts["InventoryWeapons"][0], item->getPrice());
			cout << color.get("normal");
			PrintText(35, 10, mLineTexts["InventoryWeapons"][1], item->getDmgMin());
			PrintText(35, 11, mLineTexts["InventoryWeapons"][2], item->getDmgMax());
			PrintText(35, 12, mLineTexts["InventoryWeapons"][3], item->getRequirements());
		}
		else if (item->getType() == "armor") {
			PrintBox(34, 2, 20, 6);
			PrintBox(34, 9, 20, 3);

			//Displaying Parameters ------------------
			cout << color.get("yellow");
			PrintText(35, 10, mLineTexts["InventoryArmors"][0], item->getPrice());
			cout << color.get("normal");
			PrintText(35, 11, mLineTexts["InventoryArmors"][1], item->getDmgProtection());
			PrintText(35, 12, mLineTexts["InventoryArmors"][2], item->getRequirements());
		}
		else if (item->getType() == "consumable") {
			string effect;

			PrintBox(34, 2, 20, 5);
			PrintBox(34, 8, 20, 2);

			//Displaying Parameters ------------------
			cout << color.get("yellow");
			PrintText(35, 9, mLineTexts["InventoryConsumables"][0], item->getPrice());
			cout << color.get("normal");
			{
				if (item->getEffect() == "heal") {
					effect = color.get("red") + "+" + to_string(item->getPower());
					effect += "hp" + color.get("normal");
				}
				else if (item->getEffect() == "stamina") {
					effect = color.get("green") + "+" + to_string(item->getPower());
					effect += "sp" + color.get("normal");
				}
				else if (item->getEffect() == "mana") {
					effect = color.get("blue") + "+" + to_string(item->getPower());
					effect += "mp" + color.get("normal");
				}

			}
			PrintText(35, 10, mLineTexts["InventoryConsumables"][1], effect);
		}
		else if (item->getType() == "spell") {
			string effect;
			int cost;

			PrintBox(34, 2, 20, 6);
			PrintBox(34, 9, 20, 5);

			//Displaying Parameters ------------------
			cout << color.get("yellow");
			PrintText(35, 10, mLineTexts["InventorySpells"][0], item->getPrice());
			cout << color.get("normal");

			if (item->getEffect() == "heal") {
				effect = color.get("red") + "+" + to_string(item->getPower());
				effect += "hp" + color.get("normal");
			}
			else if (item->getEffect() == "stamina") {
				effect = color.get("green") + "+" + to_string(item->getPower());
				effect += "sp" + color.get("normal");
			}
			else if (item->getEffect() == "fire") {
				effect = "+" + to_string(item->getPower());
				effect += "dmg";
			}
			PrintText(35, 11, mLineTexts["InventorySpells"][1], effect);

			//Cost -----------------------------------
			{
				cost = int(item->getMana() * ((100.0f + item->getRequirements() - hero.getWillpower()) / 100.0f));
				if (cost > item->getMana()) cost = item->getMana();
				else if (cost < item->getRequirements()) cost = item->getRequirements();
			}

			cout << color.get("blue");
			PrintText(35, 12, mLineTexts["InventorySpells"][2], cost);
			cout << color.get("normal");

			PrintText(35, 13, mLineTexts["InventorySpells"][3], item->getSpan());
			PrintText(35, 14, mLineTexts["InventorySpells"][4], item->getRequirements());
		}
	}
}

void Game::armorsMenu() {
	unsigned int choice = 0;
	bool actionExe;
	int action = 0;

	vector<shared_ptr<Item>> armors = itemTypeFind(hero.getInventory(), "armor");
	ClearConsole();

	const vector<string> blank1(6, string(20, ' '));
	const vector<string> blank2(3, string(20, ' '));
	const string blank(sLineTexts["MsgReq"].size(), ' ');

	PrintBox(34, 2, 20, 6);
	PrintBox(34, 8, 20, 3);
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["InventoryArmorsTitle"].size() / 2, 0, sLineTexts["InventoryArmorsTitle"]);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= armors.size() && action == 0) {
				DisplayGfx(35, 3, blank1);
				DisplayGfx(35, 9, blank2);

				//Requirements Message -----------------
				{
					CursorPos(55 - sLineTexts["MsgReq"].size(), 1);
					if (choice != 0 && armors[choice - 1]->getRequirements() > hero.getStrength()) {
						cout << color.get("red") << sLineTexts["MsgReq"] << color.get("normal");
					}
					else {
						cout << blank;
					}
				}

				CursorPos(2, 1);
				displayOptionsNoNum(choice, sLineTexts["Return"], armors, true);
				if (choice != 0) {
					DisplayGfx(35, 3, graphics[armors[choice - 1]->getGfxDir()]);
					cout << color.get("yellow");
					PrintText(35, 9, mLineTexts["InventoryArmors"][0], armors[choice - 1]->getPrice());
					cout << color.get("normal");
					PrintText(35, 10, mLineTexts["InventoryArmors"][1], armors[choice - 1]->getDmgProtection());
					PrintText(35, 11, mLineTexts["InventoryArmors"][2], armors[choice - 1]->getRequirements());
				}
			}
			choiceLimit(choice, armors.size() + 1);

			action = upDownEscControls(choice, armors.size() + 1);

		} while (action == 0);

		if (action == -1) {
			choice = 0;
		}

		if (choice == 0) return;

		actionExe = inventoryMenusExecution(armors[choice - 1]);

	} while (!actionExe);
}
void Game::weaponsMenu() {
	unsigned int choice = 0;
	bool actionExe;
	int action = 0;

	vector<shared_ptr<Item>> weapons = itemTypeFind(hero.getInventory(), "weapon");

	ClearConsole();

	const vector<string> blank1(5, string(20, ' '));
	const vector<string> blank2(4, string(20, ' '));
	const string blank(sLineTexts["MsgReq"].size(), ' ');

	PrintBox(34, 2, 20, 5);
	PrintBox(34, 8, 20, 4);
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["InventoryWeaponsTitle"].size() / 2, 0, sLineTexts["InventoryWeaponsTitle"]);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= weapons.size() && action == 0) {
				DisplayGfx(35, 3, blank1);
				DisplayGfx(35, 9, blank2);

				//Requirements Message -----------------
				{
					CursorPos(55 - sLineTexts["MsgReq"].size(), 1);
					if (choice != 0 && weapons[choice - 1]->getRequirements() > hero.getStrength()) {

						cout << color.get("red") << sLineTexts["MsgReq"] << color.get("normal");
					}
					else PrintText(55 - blank.size(), 1, blank);
				}

				CursorPos(2, 1);
				displayOptionsNoNum(choice, sLineTexts["Return"], weapons, true);
				if (choice != 0) {
					DisplayGfx(35, 3, graphics[weapons[choice - 1]->getGfxDir()]);
					cout << color.get("yellow");
					PrintText(35, 9, mLineTexts["InventoryWeapons"][0], weapons[choice - 1]->getPrice());
					cout << color.get("normal");
					PrintText(35, 10, mLineTexts["InventoryWeapons"][1], weapons[choice - 1]->getDmgMin());
					PrintText(35, 11, mLineTexts["InventoryWeapons"][2], weapons[choice - 1]->getDmgMax());
					PrintText(35, 12, mLineTexts["InventoryWeapons"][3], weapons[choice - 1]->getRequirements());
				}
			}
			choiceLimit(choice, weapons.size() + 1);

			action = upDownEscControls(choice, weapons.size() + 1);
		} while (action == 0);

		if (action == -1) {
			choice = 0;
		}

		if (choice == 0) return;

		actionExe = inventoryMenusExecution(weapons[choice - 1]);
	} while (!actionExe);
}
void Game::consumablesMenu() {
	unsigned int choice = 0;
	bool actionExe;
	int action = 0;

	vector<shared_ptr<Item>> consumables = itemTypeFind(hero.getInventory(), "consumable");
	ClearConsole();

	const vector<string> blank1(5, string(20, ' '));
	const vector<string> blank2(2, string(20, ' '));

	unsigned int greatestSize;
	if (sLineTexts["MsgHpFull"].size() > sLineTexts["MsgSpFull"].size()) greatestSize = sLineTexts["MsgHpFull"].size();
	else greatestSize = sLineTexts["MsgSpFull"].size();
	if (sLineTexts["MsgMpFull"].size() > greatestSize) greatestSize = sLineTexts["MsgMpFull"].size();
	const string blank(greatestSize, ' ');

	PrintBox(34, 2, 20, 5);
	PrintBox(34, 8, 20, 2);
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["InventoryConsumables"].size() / 2, 0, sLineTexts["InventoryConsumables"]);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= consumables.size() && action == 0) {
				DisplayGfx(35, 3, blank1);
				DisplayGfx(35, 9, blank2);

				//Stat Is Full Message -------------------
				{
					PrintText(55 - blank.size(), 1, blank);
					if (choice != 0 && consumables[choice - 1]->getEffect() == "heal" && hero.getHp() >= hero.getHpMax()) {
						CursorPos(55 - sLineTexts["MsgHpFull"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgHpFull"] << color.get("normal");
					}
					else if (choice != 0 && consumables[choice - 1]->getEffect() == "stamina" && hero.getSp() >= hero.getSpMax()) {
						CursorPos(55 - sLineTexts["MsgSpFull"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgSpFull"] << color.get("normal");
					}
					else if (choice != 0 && consumables[choice - 1]->getEffect() == "mana" && hero.getMp() >= hero.getMpMax()) {
						CursorPos(55 - sLineTexts["MsgMpFull"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgMpFull"] << color.get("normal");
					}
				}

				CursorPos(2, 1);
				displayOptionsNoNum(choice, sLineTexts["Return"], consumables, true);
				if (choice != 0) {
					DisplayGfx(35, 3, graphics[consumables[choice - 1]->getGfxDir()]);
					cout << color.get("yellow");
					PrintText(35, 9, mLineTexts["InventoryConsumables"][0], consumables[choice - 1]->getPrice());
					cout << color.get("normal");

					//Displaying Parameters ------------------
					{
						string effect;
						if (consumables[choice - 1]->getEffect() == "heal") {
							effect = color.get("red") + "+" + to_string(consumables[choice - 1]->getPower());
							effect += "hp" + color.get("normal");
						}
						else if (consumables[choice - 1]->getEffect() == "stamina") {
							effect = color.get("green") + "+" + to_string(consumables[choice - 1]->getPower());
							effect += "sp" + color.get("normal");
						}
						else if (consumables[choice - 1]->getEffect() == "mana") {
							effect = color.get("blue") + "+" + to_string(consumables[choice - 1]->getPower());
							effect += "mp" + color.get("normal");
						}
						PrintText(35, 10, mLineTexts["InventoryConsumables"][1], effect);
					}

				}
			}
			choiceLimit(choice, consumables.size() + 1);

			action = upDownEscControls(choice, consumables.size() + 1);
		} while (action == 0);

		if (action == -1) {
			choice = 0;
		}
		if (choice == 0) return;

		actionExe = inventoryMenusExecution(consumables[choice - 1]);
	} while (!actionExe);
}
const bool Game::spellsMenu(Enemy &enemy) {
	unsigned int choice = 0;
	int cost = NULL;
	bool actionExe;
	int action = 0;

	vector<shared_ptr<Item>> spells = itemTypeFind(hero.getInventory(), "spell");
	ClearConsole();

	const vector<string> blank1(6, string(20, ' '));
	const vector<string> blank2(4, string(20, ' '));

	unsigned int greatestSize;
	if (sLineTexts["MsgHpFull"].size() > sLineTexts["MsgSpFull"].size()) greatestSize = sLineTexts["MsgHpFull"].size();
	else greatestSize = sLineTexts["MsgSpFull"].size();
	if (sLineTexts["MsgMpFull"].size() > greatestSize) greatestSize = sLineTexts["MsgMpFull"].size();
	const string blank(greatestSize, ' ');

	PrintBox(34, 2, 20, 6);
	PrintBox(34, 9, 20, 4);
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["InventorySpellsTitle"].size() / 2, 0, sLineTexts["InventorySpellsTitle"]);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= spells.size() && action == 0) {
				DisplayGfx(35, 3, blank1);
				DisplayGfx(35, 10, blank2);

				//Stat Is Full or Empty Message ----------
				{
					PrintText(55 - blank.size(), 1, blank);
					if (choice != 0 && spells[choice - 1]->getEffect() == "heal" && hero.getHp() >= hero.getHpMax()) {
						CursorPos(55 - sLineTexts["MsgHpFull"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgHpFull"] << color.get("normal");
					}
					else if (choice != 0 && spells[choice - 1]->getEffect() == "stamina" && hero.getSp() >= hero.getSpMax()) {
						CursorPos(55 - sLineTexts["MsgSpFull"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgSpFull"] << color.get("normal");
					}
					else if (choice != 0 && hero.getMp() <= 0 && (spells[choice - 1]->getEffect() == "heal" || enemy.getName() != "NONE")) {
						CursorPos(55 - sLineTexts["MsgMpEmpty"].size(), 1);
						cout << color.get("red") << sLineTexts["MsgMpEmpty"] << color.get("normal");
					}
				}

				CursorPos(2, 1);
				displayOptionsNoNum(choice, sLineTexts["Return"], spells, false);
				if (choice != 0) {
					DisplayGfx(35, 3, graphics[spells[choice - 1]->getGfxDir()]);

					//Displaying Parameters ------------------
					{
						string effect;
						if (spells[choice - 1]->getEffect() == "heal") {
							effect = color.get("red") + "+" + to_string(spells[choice - 1]->getPower());
							effect += "hp" + color.get("normal");
						}
						else if (spells[choice - 1]->getEffect() == "stamina") {
							effect = color.get("green") + "+" + to_string(spells[choice - 1]->getPower());
							effect += "sp" + color.get("normal");
						}
						else if (spells[choice - 1]->getEffect() == "fire") {
							effect = "+" + to_string(spells[choice - 1]->getPower());
							effect += "dmg";
						}
						PrintText(35, 10, mLineTexts["InventorySpells"][1], effect);

						//Cost -----------------------------------
						{
							cost = int(spells[choice - 1]->getMana() * ((100.0f + spells[choice - 1]->getRequirements() - hero.getWillpower()) / 100.0f));
							if (cost > spells[choice - 1]->getMana()) cost = spells[choice - 1]->getMana();
							else if (cost < spells[choice - 1]->getRequirements()) cost = spells[choice - 1]->getRequirements();
						}

						cout << color.get("blue");
						PrintText(35, 11, mLineTexts["InventorySpells"][2], cost);
						cout << color.get("normal");
						PrintText(35, 12, mLineTexts["InventorySpells"][3], spells[choice - 1]->getSpan());
						PrintText(35, 13, mLineTexts["InventorySpells"][4], spells[choice - 1]->getRequirements());
					}
				}
			}
			choiceLimit(choice, spells.size() + 1);
			action = upDownEscControls(choice, spells.size() + 1);
		} while (action == 0);

		if (action == -1) {
			choice = 0;
		}

		if (choice == 0) return false;

		if (enemy.getName() == "NONE") {
			actionExe = inventoryMenusExecution(spells[choice - 1]);
		}
		else {
			actionExe = spellsExecution(choice, spells[choice - 1], enemy);
		}

	} while (!actionExe);

	return true;
}

const bool Game::inventoryMenuExecution(unsigned int &choice) {
	switch (choice) {
	case 0: {
		return true;
	}
	case 1: {
		weaponsMenu();
		break;
	}
	case 2: {
		armorsMenu();
		break;
	}
	case 3: {
		consumablesMenu();
		break;
	}
	case 4: {
		spellsMenu(noneEnemy);
		break;
	}

	default:
		return true;
	}

	choice = 0;
	return false;
}
void Game::inventoryMenu() {
	unsigned int choice = 0;
	int action;

	//Looping Until Return in Execution ------
	do {
		ClearConsole();
		PrintBox(0, 0, 54, 17);
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice < mLineTexts["Inventory"].size()) {
				displayOptionsMenu(choice, sLineTexts["InventoryTitle"], mLineTexts["Inventory"]);
			}
			choiceLimit(choice, mLineTexts["Inventory"].size());
			action = upDownEscControls(choice, mLineTexts["Inventory"].size());
		} while (action == 0);
		hero.removeFromInventory();
		if (action == -1) {
			choice = 0;
		}

	} while (!inventoryMenuExecution(choice));
}

void Game::shoppingBuyMenu(vector<shared_ptr<Item>> items) {
	unsigned int choice = 0;
	int action = 0;

	const vector<string> blank(15, string(21, ' '));

	unsigned int greatestSize;
	if (sLineTexts["MsgHaveSpell"].size() > sLineTexts["MsgMoney"].size()) greatestSize = sLineTexts["MsgMoney"].size();
	else greatestSize = sLineTexts["MsgMoney"].size();
	const string blankLine(greatestSize, ' ');

	//Removing Spells Hero Already Has -------
	{
		for (const shared_ptr<Item> &item : hero.getInventory()) {
			for (auto it = items.begin(); it != items.end(); ++it) {
				if ((*it)->getType() == "spell" && (*it)->getNameRaw() == item->getNameRaw()) {
					items.erase(it);
					break;
				}
			}
		}
	}

	ClearConsole();
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["ShoppingBuyTitle"].size() / 2, 0, sLineTexts["ShoppingBuyTitle"]);
	displayInvGfx(nullptr);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= items.size() && action == 0) {
				PrintText(55 - blankLine.size(), 1, blankLine);
				DisplayGfx(34, 2, blank);
				CursorPos(2, 2);
				displayOptionsNoNum(choice, sLineTexts["Return"], items, false);
			}
			if (choice > 0 && choice <= items.size()) {
				displayInvGfx(items[choice - 1]);

				//Displaying Messages --------------------
				PrintText(55 - blankLine.size(), 1, blankLine);
				if (items[choice - 1]->getType() == "spell" && find(hero.getInventory().begin(), hero.getInventory().end(), items[choice - 1]) != hero.getInventory().end()) {
					CursorPos(55 - sLineTexts["MsgHaveSpell"].size(), 1);
					cout << color.get("red") << sLineTexts["MsgHaveSpell"] << color.get("normal");
				}
				else if (hero.getMoney() < items[choice - 1]->getPrice()) {
					CursorPos(55 - sLineTexts["MsgMoney"].size(), 1);
					cout << color.get("red") << sLineTexts["MsgMoney"] << color.get("normal");
				}
			}
			choiceLimit(choice, items.size() + 1);

			action = upDownEscControls(choice, items.size() + 1);
		} while (action == 0);

		if (action == 1 && choice > 0) {
			if (items[choice - 1]->getType() == "spell" && find(hero.getInventory().begin(), hero.getInventory().end(), items[choice - 1]) != hero.getInventory().end()){}
			else if (hero.getMoney() >= items[choice - 1]->getPrice()) {
				hero.decreaseMoney(items[choice - 1]->getPrice());
				hero.addToInventory(items[choice - 1]);
				audio.character("Trade1");
			}

		}
		if (action == -1) {
			choice = 0;
		}

		if (choice == 0) return;

	} while (true);
}
void Game::shoppingSellMenu() {
	unsigned int choice = 0;
	int action = 0;

	const vector<string> blank(15, string(21, ' '));

	vector<shared_ptr<Item>> items = hero.getInventory();
	vector<shared_ptr<Item>> spells;

	//Spell Removing -------------------------
	bool removed;
	do {
		removed = false;
		for (auto item = items.begin(); item != items.end(); ++item) {
			if ((*item)->getType() == "spell") {
				spells.push_back(*item);
				items.erase(item);
				removed = true;
				break;
			}
		}
	} while (removed);

	ClearConsole();
	PrintBox(0, 0, 54, 17);
	PrintText(28 - sLineTexts["ShoppingSellTitle"].size() / 2, 0, sLineTexts["ShoppingSellTitle"]);
	displayInvGfx(nullptr);

	do {
		//Displaying Until Enter -----------------
		do {
			if (choice >= 0 && choice <= items.size() && action == 0) {
				DisplayGfx(34, 1, blank);
				CursorPos(2, 2);
				displayOptionsNoNum(choice, sLineTexts["Return"], items, true);
			}
			if (choice > 0 && choice <= items.size()) displayInvGfx(items[choice - 1]);
			choiceLimit(choice, items.size() + 1);

			action = upDownEscControls(choice, items.size() + 1);
		} while (action == 0);
		if (action == 1 && choice > 0 && items[choice - 1]->getType() != "spell") {
			hero.addMoney(items[choice - 1]->getPrice());
			items[choice - 1]->decreaseNumber();
			action = 0;
			audio.character("Trade1");
			if (items[choice - 1]->getNumber() == 0) {
				items.erase(items.begin() + choice - 1);
				choice = 0;
				ClearConsole();
				PrintBox(0, 0, 54, 17);
				PrintText(28 - sLineTexts["ShoppingBuyTitle"].size() / 2, 0, sLineTexts["ShoppingBuyTitle"]);
				displayInvGfx(nullptr);
				continue;
			}
		}
		if (action == -1) {
			choice = 0;
		}

		if (choice == 0) {
			//Spell Re-adding ------------------------
			for (const shared_ptr<Item> &spell : spells) {
				items.push_back(spell);
			}
			hero.replaceInventory(items);

			return;
		}

	} while (true);
}
void Game::shoppingChoiceMenu(const vector<shared_ptr<Item>> &items) {
	unsigned int choice = 0;
	int action = 0;

	ClearConsole();
	setFontSize(globalFontSize + 32);

	do {
		//Displaying Until Enter Or Esc-----------
		PrintBox(0, 0, 54, 17);
		do {
			if (choice >= 0 && choice <= 2 && action == 0) displayOptionsMenu(choice, sLineTexts["ShoppingTitle"], mLineTexts["Shopping"]);
			choiceLimit(choice, 3);

			action = upDownEscControls(choice, 3);
		} while (action == 0);

		if (action == 1 && choice != 0) {
			const int choiceTemp = choice;

			if (choice == 1) shoppingBuyMenu(items);
			else if (choice == 2) shoppingSellMenu();

			choice = choiceTemp;
			action = 0;
			ClearConsole();
		}

		if (action == -1) {
			choice = 0;
		}
		if (choice == 0) break;

	} while (true);

	ClearConsole();
	setFontSize(globalFontSize + storyFontInc);
}