#pragma once

#include "armor.hpp"
#include "consumable.hpp"
#include "global_headers.h"
#include "item.hpp"
#include "spell.hpp"
#include "weapon.hpp"

class consumable;

class character
{
protected:
	std::string type_{};

	// Location -------------------------------
	int location_;
	int sub_location_;

	// Leveling -------------------------------
	int level_;
	int exp_;
	int exp_next_;

	std::string name_{};

	// Stats1 ---------------------------------
	int hp_; // Health
	int sp_; // Stamina
	int mp_; // Mana
	int hp_max_;
	int sp_max_;
	int mp_max_;

	// Dependent stats ------------------------
	int base_dmg_min_;
	int base_dmg_max_;
	int dmg_min_;
	int dmg_max_;
	int base_defence_;
	int defence_;
	int evasion_chance_;
	int hit_chance_;

	// Stats2 ---------------------------------
	int strength_; // Increases base dmg, allows using better weapons
	int endurance_; // Increases base defence
	int willpower_; // Allows using better spells and decreases play_spell_sound cast cost
	int agility_; // Increases play_evasion_sound and hit chance, decreases stamina cost

	int stat1_points_;
	int stat2_points_;

	// Equipment -----------------------------
	std::vector<std::shared_ptr<item>> inventory_;
	weapon weapon_{};
	armor armor_{};

	std::vector<spell> active_spells_{};

	int money_ = 0;

	// Constructor and a destructor ----------
public:
	character();
	virtual ~character();

	// Functions ------------------------------
	void clear();
	void refresh_item_dependent() noexcept;
	virtual void refresh_stat_dependent() noexcept;

	void stats1_setup();
	auto stats2_setup() -> void;
	void initialize(const std::string& name);

	void display() const;
	void display_dependent() const;

	void add_exp(const int& exp);
	void level_up();

	// Modifiers ------------------------------
	void set_location(const int& var) noexcept;
	void set_sub_location(const int& var) noexcept;
	void add_to_inventory(const std::shared_ptr<item>& item1);
	void add_to_inventory(const weapon& weapon1);
	void add_to_inventory(const armor& armor1);
	void add_to_inventory(const consumable& consumable1);
	void add_to_inventory(const consumable& consumable1, const unsigned int& num);
	void add_to_inventory(const spell& item);
	void remove_from_inventory();

	void equip(const std::shared_ptr<item>& item);

private:
	void equip(const weapon& item);
	void equip(const armor& item);

public:
	void consume(const std::shared_ptr<item>& item);
	void consume(const consumable& item);

	void sort_inventory();

	void add_to_active_spells(const spell& spell);
	void active_spells_refresh();
	void cast_spell(const spell& spell, const std::shared_ptr<character>& enemy);
private:
	void cast_spell(const spell& spell);
	void get_spell(const spell& spell);
public:
	void clear_active_spells() noexcept;

	/*------------------------------ Accessors ------------------------------*/
	const std::string& get_type() const noexcept;

	// Location Accessors ---------------------
	const int& get_location() const noexcept;
	const int& get_sub_location() const noexcept;

	const std::string& get_name() const noexcept;

	// Levelling Accessors---------------------
	const int& get_level() const noexcept;
	const int& get_exp() const noexcept;
	const int& get_exp_next() const noexcept;

	// Stats1 Accessors -----------------------
	const int& get_hp() const noexcept;
	const int& get_hp_max() const noexcept;
	const int& get_sp() const noexcept;
	const int& get_sp_max() const noexcept;
	const int& get_mp() const noexcept;
	const int& get_mp_max() const noexcept;

	// Dependent stats Accessors --------------
	const int& get_base_dmg_min() const noexcept;
	const int& get_base_dmg_max() const noexcept;
	const int& get_dmg_min() const noexcept;
	const int& get_dmg_max() const noexcept;
	const int& get_defence() const noexcept;
	const int& get_hit_c() const noexcept;
	const int& get_evasion_c() const noexcept;

	// Stats2 Accessors -----------------------
	const int& get_strength() const noexcept;
	const int& get_endurance() const noexcept;
	const int& get_willpower() const noexcept;
	const int& get_agility() const noexcept;
	const int& get_stat1_points() const noexcept;
	const int& get_stat2_points() const noexcept;

	// Equipment Accessors --------------------
	const std::vector<std::shared_ptr<item>>& get_inventory() const noexcept;
	std::vector<std::shared_ptr<item>>& get_inventory() noexcept;
	const weapon& get_weapon() const noexcept;
	const armor& get_armor() const noexcept;
	armor& get_armor() noexcept;
	const int& get_money() const noexcept;
	/*-----------------------------------------------------------------------*/


	// Saving|Loading -------------------------
	void save() const;
	void load(const std::string& name);

	// Other Modifiers ------------------------
	void increase_hp(const int& inc) noexcept;
	void increase_sp(const int& inc) noexcept;
	void increase_mp(const int& inc) noexcept;
	void get_damaged(const int& dmg) noexcept;
	void decrease_sp(const int& dec) noexcept;
	void decrease_money(const int& dec) noexcept;
	void increase_def(const int& inc) noexcept;
	void increase_ev(const int& inc) noexcept;
	void add_money(const int& inc) noexcept;
	void replace_inventory(const std::vector<std::shared_ptr<item>>& items);

	const std::vector<spell>& get_active_spells() const noexcept;
};
