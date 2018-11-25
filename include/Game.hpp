#pragma once
#include "FrequentHeaders.h"
#include "Character.hpp"
#include "Items.hpp"

class Game
{
	//Fields ---------------------------------
private:
	bool playing;	//If false the game is closed
	bool inSession; //It indicates if player is actually playing
	bool plotSwitches[10]{};
	json scenario{};

	//Character ------------------------------
	Character hero;
	Character tempHero;

	//Constructors and a destructor ----------
public:
	Game() noexcept;
	virtual ~Game() = default;

private:
	/*------------------------------ Functions ------------------------------*/
	//Safeguards and initialization ----------
	const bool nameUsed(const std::string & name) const;
	const unsigned int countSaves() const;
	void loadResourceMaps() const;
	void initialize();

	//Saving and Loading ---------------------
	void load(const std::string &filename);
	void deleteSavesMenu();
	void loadMenu();
	void heroToTempHero(); //In this function the hero is copied to tempHero with additional operations

	/*------------------------------ Inventory ------------------------------*/
	const bool inventoryMenusExecution(const unsigned int& choice, const std::shared_ptr<Item> &item);

	void displayInvGfx(const std::shared_ptr<Item> &item);

	void armorsMenu();
	void weaponsMenu();
	void consumablesMenu();
	const bool spellsMenu(Enemy &enemy);

	const bool inventoryMenuExecution(unsigned int& choice);
	void inventoryMenu();

	void shoppingBuyMenu(std::vector<std::shared_ptr<Item>> items);
	void shoppingSellMenu();
	void shoppingChoiceMenu(const std::vector<std::shared_ptr<Item>> &items);
	/*-----------------------------------------------------------------------*/

	//Main and Pause Menus -------------------
	const bool pauseMenuExecution(const unsigned int &choice);
	void pauseMenu();

	void refreshCodePage();
	void saveSettings();
	void loadSettings();
	void settingsMenu();

	void loadScenario();
	void scenarioLoadMenu();

	void mainMenuExecution(unsigned int &choice);
	void logo();
public:
	void mainMenu();

private:
	//Battle System --------------------------
	const bool spellsExecution(const unsigned int & choice, const std::shared_ptr<Item> &item, Enemy &enemy);
	void displayFightStats(const Enemy& enemy);
	void fightExecution(Enemy &enemy);
	const bool fightExecution(const unsigned int& choice, Enemy &enemy);
	void fight(Enemy enemy);

	//Story ----------------------------------
	const unsigned int y1 = 22;

	void printMenuGFX(const std::vector<std::string> &description, const std::vector<std::string> &gfx);
	void locationMenu(unsigned int &choice, const std::vector<std::string> &description, const std::vector<std::string> &options, const std::vector<std::string> &gfx);
	void locationActionGfx(const std::vector<std::string> &description);
	void jsonToStory();

	//Accessors ------------------------------
	const bool& getPlaying() const noexcept { return this->playing; }
};