#pragma once

#include "character.hpp"

class enemy;

class game final
{
	// Fields ---------------------------------
private:
	bool playing_; // If false the game is closed
	bool in_session_; // It indicates if player is actually playing
	bool plot_switches_[10]{};
	nlohmann::json scenario_{};

	// Character ------------------------------
	character hero_{};
	character temp_hero_{};

	// Constructors and destructor ------------
public:
	game();
	~game() = default;

private:
	/*------------------------------ Functions ------------------------------*/
	// Safeguards and initialization ----------
	static const bool name_used(const std::string& name);
	const unsigned int count_saves() const;
	static void load_resource_maps();
	void initialize();

	// Saving and Loading ---------------------
	void load(const std::string& filename);
	void delete_saves_menu() const;
	void load_menu();
	void hero_to_temp_hero(); // In this function the hero is copied to tempHero with additional operations

	/*------------------------------ Inventory ------------------------------*/
	const bool inventory_menus_execution(const std::shared_ptr<item>& item1);

	void display_inv_gfx(const std::shared_ptr<item>& item1) const;

	void armors_menu();
	void weapons_menu();
	void consumables_menu();
	const bool spells_menu(enemy& enemy);

	const bool inventory_menu_execution(unsigned int& choice);
	void inventory_menu();

	void shopping_buy_menu(std::vector<std::shared_ptr<item>> items);
	void shopping_sell_menu();
	void shopping_choice_menu(const std::vector<std::shared_ptr<item>>& items);
	/*-----------------------------------------------------------------------*/

	// Main and Pause Menus -------------------
	const bool pause_menu_execution(const unsigned int& choice);
	void pause_menu();

	static void refresh_code_page();
	static void save_settings();
	static void load_settings();
	void settings_menu();

	void load_scenario();
	void scenario_load_menu();

	void main_menu_execution(unsigned int& choice);
	void logo() const;

public:
	void main_menu();

private:
	// Battle System --------------------------
	const bool spells_execution(const unsigned int& choice, const std::shared_ptr<spell>& spell1, enemy& enemy1);
	void display_fight_stats(const enemy& enemy1) const;
	void fight_execution(enemy& enemy1);
	const bool fight_execution(const unsigned int& choice, enemy& enemy1);
	void fight(enemy enemy1);

	// Story ----------------------------------
	const unsigned int y1_ = 22;

	void print_menu_gfx(const std::vector<std::string>& description, const std::vector<std::string>& gfx) const;
	void location_menu(unsigned int& choice, const std::vector<std::string>& description,
	                   const std::vector<std::string>& options, const std::vector<std::string>& gfx);
	void location_action_gfx(const std::vector<std::string>& description) const;
	void json_to_story();

	// Accessors ------------------------------
	const bool& get_playing() const { return this->playing_; }
};
