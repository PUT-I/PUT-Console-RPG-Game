#include "character.hpp"

#include "color_util.hpp"
#include "console_util.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"
#include "interface_util.hpp"
#include "sound_manager.hpp"

using namespace std;

character::character()
{
	type_ = "character";

	// Location -------------------------------
	this->location_ = 0;
	this->sub_location_ = 0;

	// Leveling -------------------------------
	{
		this->level_ = 0;
		this->exp_ = 0;
		this->exp_next_ = 0;
	}

	this->name_ = "NONE";
	// Stats1 ---------------------------------
	{
		this->hp_ = 0;
		this->sp_ = 0;
		this->mp_ = 0;
		this->hp_max_ = 0;
		this->sp_max_ = 0;
		this->mp_max_ = 0;
	}

	// Dependent stats ------------------------
	{
		this->base_dmg_min_ = 0;
		this->base_dmg_max_ = 0;
		this->dmg_min_ = 0;
		this->dmg_max_ = 0;
		this->base_defence_ = 0;
		this->evasion_chance_ = 0;
		this->hit_chance_ = 0;
		this->base_defence_ = 0;
		this->defence_ = 0;
	}

	// Stats2 ---------------------------------
	{
		this->strength_ = 0;
		this->endurance_ = 0;
		this->willpower_ = 0;
		this->agility_ = 0;
	}

	this->stat1_points_ = 0;
	this->stat2_points_ = 0;

	this->weapon_;
	this->armor_;
}

character::~character() = default;

void character::set_location(const int& var) noexcept
{
	this->location_ = var;
}

void character::set_sub_location(const int& var) noexcept
{
	this->sub_location_ = var;
}

const string& character::get_type() const noexcept
{
	return type_;
}

const int& character::get_location() const noexcept
{
	return this->location_;
}

const int& character::get_sub_location() const noexcept
{
	return this->sub_location_;
}

const string& character::get_name() const noexcept
{
	return this->name_;
}

const int& character::get_level() const noexcept
{
	return this->level_;
}

const int& character::get_exp() const noexcept
{
	return this->exp_;
}

const int& character::get_exp_next() const noexcept
{
	return this->exp_next_;
}

const int& character::get_hp() const noexcept
{
	return this->hp_;
}

const int& character::get_hp_max() const noexcept
{
	return this->hp_max_;
}

const int& character::get_sp() const noexcept
{
	return this->sp_;
}

const int& character::get_sp_max() const noexcept
{
	return this->sp_max_;
}

const int& character::get_mp() const noexcept
{
	return this->mp_;
}

const int& character::get_mp_max() const noexcept
{
	return this->mp_max_;
}

const int& character::get_base_dmg_min() const noexcept
{
	return this->base_dmg_min_;
}

const int& character::get_base_dmg_max() const noexcept
{
	return this->base_dmg_max_;
}

const int& character::get_dmg_min() const noexcept
{
	return this->dmg_min_;
}

const int& character::get_dmg_max() const noexcept
{
	return this->dmg_max_;
}

const int& character::get_defence() const noexcept
{
	return this->defence_;
}

const int& character::get_hit_c() const noexcept
{
	return this->hit_chance_;
}

const int& character::get_evasion_c() const noexcept
{
	return this->evasion_chance_;
}

const int& character::get_strength() const noexcept
{
	return this->strength_;
}

const int& character::get_endurance() const noexcept
{
	return this->endurance_;
}

const int& character::get_willpower() const noexcept
{
	return this->willpower_;
}

const int& character::get_agility() const noexcept
{
	return this->agility_;
}

const int& character::get_stat1_points() const noexcept
{
	return this->stat1_points_;
}

const int& character::get_stat2_points() const noexcept
{
	return this->stat2_points_;
}

const vector<shared_ptr<item>>& character::get_inventory() const noexcept
{
	return this->inventory_;
}

vector<shared_ptr<item>>& character::get_inventory() noexcept
{
	return this->inventory_;
}

const weapon& character::get_weapon() const noexcept
{
	return this->weapon_;
}

const armor& character::get_armor() const noexcept
{
	return this->armor_;
}

armor& character::get_armor() noexcept
{
	return this->armor_;
}

const int& character::get_money() const noexcept
{
	return money_;
}

void character::replace_inventory(const vector<shared_ptr<item>>& items)
{
	inventory_ = items;
}

const vector<spell>& character::get_active_spells() const noexcept
{
	return active_spells_;
}

// Functions ------------------------------
void character::clear()
{
	inventory_.clear();
	active_spells_.clear();
	weapon_ = weapon();
	armor_ = armor();
}

void character::refresh_item_dependent() noexcept
{
	this->dmg_min_ = base_dmg_min_ + weapon_.get_dmg_min();
	this->dmg_max_ = base_dmg_max_ + weapon_.get_dmg_max();
	this->defence_ = base_defence_ + armor_.get_dmg_protection();
}

void character::refresh_stat_dependent() noexcept
{
	this->base_dmg_min_ = strength_;
	this->base_dmg_max_ = strength_ * 2;
	this->hit_chance_ = 48 + agility_ * 2;
	this->evasion_chance_ = agility_;
	this->base_defence_ = endurance_ * 2;
}

void character::stats1_setup()
{
	// Declarations ----------------------------
	unsigned int choice = 0;
	const int stat1_points_prev = stat1_points_;
	int result;

	const vector<int*> stats{&hp_max_, &sp_max_, &mp_max_};
	const vector<int> stats_prev{hp_max_, sp_max_, mp_max_}; // Used to prevent decreasing stat below its prev number

	// Stat Names and Setting Colors -----------
	string health{color.get("red")};
	string stamina{color.get("green")};
	string mana{color.get("blue")};

	// Adding Option Names ---------------------
	{
		health += m_line_texts["Stats1Names"][0];
		stamina += m_line_texts["Stats1Names"][1];
		mana += m_line_texts["Stats1Names"][2];
	}

	const vector<string> options{health, stamina, mana, s_line_texts["Continue"]};

	// Interface ------------------------------
	clear_console();
	print_box(0, 0, 54, 17);
	print_text(27 - s_line_texts["StatsTitle"].size() / 2, 0, s_line_texts["StatsTitle"]);
	do
	{
		if (choice >= 0 && choice < options.size())
		{
			refresh_stat_dependent();

			cursor_pos(8, 2);
			cout << s_line_texts["AvailablePoints"] << stat1_points_ << "    ";
			cursor_pos(0, 4);
			display_options_no_num(choice, options, stats);
		}
		choice_limit(choice, options.size());
		result = up_down_left_right_controls(choice, options.size(), stat1_points_, stat1_points_prev, stats,
		                                     stats_prev, 10);
		if (result == -1)
		{
			clear_console();
			print_box(0, 0, 54, 17);
			print_text(8, 4, s_line_texts["SpendAllPoints"]);

			cursor_pos(8, 6);
			space_pause();
			clear_console();
			print_box(0, 0, 54, 17);
			print_text(27 - s_line_texts["StatsTitle"].size() / 2, 0, s_line_texts["StatsTitle"]);
		}
	}
	while (result != 1);
	sounds.play_sound(sound_manager::enter);

	this->hp_ = this->hp_max_;
	this->sp_ = this->sp_max_;
	this->mp_ = this->mp_max_;
}

void character::stats2_setup()
{
	// Declarations ----------------------------
	unsigned int choice = 0;
	int result;
	const int stat2_points_prev = stat2_points_;
	const vector<int*> stats{&strength_, &endurance_, &willpower_, &agility_};
	const vector<int> stats_prev{strength_, endurance_, willpower_, agility_};
	// Used to prevent decreasing stat below its prev number

	vector<string> options = m_line_texts["Stats2Names"];
	options.push_back(s_line_texts["Continue"]);

	// Interface ------------------------------
	clear_console();
	print_box(0, 0, 54, 17);
	print_text(27 - s_line_texts["StatsTitle"].size() / 2, 0, s_line_texts["StatsTitle"]);
	do
	{
		if (choice >= 0 && choice < options.size())
		{
			refresh_stat_dependent();
			cursor_pos(8, 2);
			cout << s_line_texts["AvailablePoints"] << stat2_points_ << "    ";
			cursor_pos(0, 4);
			display_dependent();
			cursor_move(0, 2);
			display_options_no_num(choice, options, stats);
		}

		choice_limit(choice, options.size());
		result = up_down_left_right_controls(choice, options.size(), stat2_points_, stat2_points_prev, stats,
		                                     stats_prev,
		                                     1);
		if (result == -1)
		{
			clear_console();
			print_box(0, 0, 54, 17);
			print_text(8, 4, s_line_texts["SpendAllPoints"]);

			cursor_pos(8, 6);
			space_pause();
			clear_console();
			print_box(0, 0, 54, 17);
			print_text(27 - s_line_texts["StatsTitle"].size() / 2, 0, s_line_texts["StatsTitle"]);
		}
	}
	while (result != 1);
}

void character::initialize(const string& name)
{
	type_ = "character";

	// Location -------------------------------
	this->location_ = 1;
	this->sub_location_ = 1;

	// Leveling -------------------------------
	{
		this->level_ = 1;
		this->exp_ = 0;
		this->exp_next_ = 100;
	}

	this->name_ = name;
	// Stats1 ---------------------------------
	{
		this->hp_max_ = 100;
		this->sp_max_ = 100;
		this->mp_max_ = 100;
	}

	// Stats2 ---------------------------------
	{
		this->strength_ = 1;
		this->endurance_ = 1;
		this->willpower_ = 1;
		this->agility_ = 1;
	}

	this->stat1_points_ = 3;
	this->stat2_points_ = 10;

	stats1_setup();
	stats2_setup();

	this->weapon_ = items::weapons["NONE"];
	this->armor_ = items::armors["Clothes"];
	this->armor_.decrease_number();

	// Refreshing -----------------------------
	{
		refresh_stat_dependent();
		refresh_item_dependent();
	}
}

void character::display() const
{
	const unsigned int y1 = 0;
	const unsigned int x1 = 1;
	const unsigned int x2 = 31;

	const vector<unsigned int> y{
		y1 + 6, y1 + 3, y1 + 11, y1 + 6, y1 + 12
	};

	// Name and Leveling ----------------------
	{
		cursor_pos(x1, y[0]);
		cout << m_line_texts["SheetNandL"][0] << name_;
		cursor_pos(x1, y[0] + 1);
		cout << m_line_texts["SheetNandL"][1] << level_;
		cursor_pos(x1, y[0] + 2);
		cout << m_line_texts["SheetNandL"][2] << exp_ << "/" << exp_next_;
		cursor_pos(x1, y[0] + 3);
		cout << color.get("yellow") << m_line_texts["SheetNandL"][3] << money_ << color.get("normal");

		print_box(x1 - 1, y[0] - 1, 29, 4);
	}

	// Equipment ------------------------------
	{
		cursor_pos(x1, y[1]);
		cout << m_line_texts["SheetEquipment"][0] << weapon_.get_name_localization();
		cursor_pos(x1, y[1] + 1);
		cout << m_line_texts["SheetEquipment"][1] << armor_.get_name_localization();

		print_box(x1 - 1, y[1] - 1, 59, 2);
	}

	// Stats1 ---------------------------------
	{
		cursor_pos(x1, y[2]);
		cout << color.get("red") << m_line_texts["Stats1Names"][0] << hp_ << "/" << hp_max_ << color.get("normal");
		cursor_pos(x1, y[2] + 1);
		cout << color.get("green") << m_line_texts["Stats1Names"][1] << sp_ << "/" << sp_max_ << color.get("normal");
		cursor_pos(x1, y[2] + 2);
		cout << color.get("blue") << m_line_texts["Stats1Names"][2] << mp_ << "/" << mp_max_ << color.get("normal");

		print_box(x1 - 1, y[2] - 1, 29, 3);
	}

	// Dependent Stats ------------------------
	{
		cursor_pos(x2, y[3]);
		cout << m_line_texts["StatsDepNames"][0] << dmg_min_;
		cursor_pos(x2, y[3] + 1);
		cout << m_line_texts["StatsDepNames"][1] << dmg_max_;
		cursor_pos(x2, y[3] + 2);
		cout << m_line_texts["StatsDepNames"][2] << defence_;
		cursor_pos(x2, y[3] + 3);
		cout << m_line_texts["StatsDepNames"][3] << hit_chance_;
		cursor_pos(x2, y[3] + 4);
		cout << m_line_texts["StatsDepNames"][4] << evasion_chance_;

		print_box(x2 - 1, y[3] - 1, 29, 5);
	}

	// Stats2 ---------------------------------
	{
		cursor_pos(x2, y[4]);
		cout << m_line_texts["Stats2Names"][0] << strength_;
		cursor_pos(x2, y[4] + 1);
		cout << m_line_texts["Stats2Names"][1] << endurance_;
		cursor_pos(x2, y[4] + 2);
		cout << m_line_texts["Stats2Names"][2] << willpower_;
		cursor_pos(x2, y[4] + 3);
		cout << m_line_texts["Stats2Names"][3] << agility_;

		print_box(x2 - 1, y[4] - 1, 29, 4);
	}

	// Title ----------------------------------
	{
		print_text(30 - s_line_texts["SheetTitle"].size() / 2, y[1] - 1, s_line_texts["SheetTitle"]);
	}

	// Space Pause Position -------------------
	cursor_pos(1, y[2] + 4);
}

void character::display_dependent() const
{
	const int cursor_prev_y = get_cursor_pos().Y;

	cursor_pos(8, cursor_prev_y);
	cout << m_line_texts["StatsDepNames"][0] << base_dmg_min_ << "  ";
	cursor_pos(8, cursor_prev_y + 1);
	cout << m_line_texts["StatsDepNames"][1] << base_dmg_max_ << "  ";
	cursor_pos(8, cursor_prev_y + 2);
	cout << m_line_texts["StatsDepNames"][2] << base_defence_ << "  ";
	cursor_pos(8, cursor_prev_y + 3);
	cout << m_line_texts["StatsDepNames"][3] << hit_chance_ << "  ";
	cursor_pos(8, cursor_prev_y + 4);
	cout << m_line_texts["StatsDepNames"][4] << evasion_chance_ << "  ";
}

void character::add_exp(const int& exp)
{
	const string full_msg{
		name_ + m_line_texts["MsgGain"][0] + color.get("yellow") + to_string(exp)
		+ m_line_texts["MsgGain"][1] + color.get("normal")
	};

	clear_console();
	print_box(0, 0, 54, 17);
	cursor_pos(8, 4);
	cout << full_msg;
	exp_ += exp;
	if (exp_ >= exp_next_)
	{
		sounds.play_sound(sound_manager::misc, "LevelUp");
		level_up();
	}
	else
	{
		cursor_pos(8, 6);
		space_pause();
	}
	clear_console();
}

void character::level_up()
{
	// Text -----------------------------------
	{
		cursor_pos(8, 6);
		cout << s_line_texts["MsgLevelUp"];
		cursor_pos(8, 8);
		space_pause();
	}

	while (exp_ >= exp_next_)
	{
		this->level_++;

		// Points increasing ----------------------
		{
			if (level_ < 10)
			{
				this->stat1_points_ += 3;
				this->stat2_points_ += 2;
			}
			else if (10 <= level_ && level_ < 20)
			{
				this->stat1_points_ += 5;
				this->stat2_points_ += 3;
			}
			else
			{
				this->stat1_points_ += 10;
				this->stat2_points_ += 5;
			}
		}

		this->exp_ -= exp_next_;
		this->exp_next_ += 100;
	}
	stats1_setup();
	stats2_setup();
}

// Modifiers ------------------------------
void character::add_to_inventory(const shared_ptr<item>& item1)
{
	if (!item1) return;

	if (inventory_.size() != 0)
	{
		for (const shared_ptr<item>& e : inventory_)
		{
			if (e->get_name_localization() == item1->get_name_localization())
			{
				e->increase_number();
				return;
			}
		}
	}

	this->inventory_.push_back(item1);
	sort_inventory();
}

void character::add_to_inventory(const weapon& weapon1)
{
	if (inventory_.size() != 0)
	{
		for (const shared_ptr<item>& e : inventory_)
		{
			if (e->get_name_localization() == weapon1.get_name_localization())
			{
				e->increase_number();
				return;
			}
		}
	}
	this->inventory_.push_back(make_shared<weapon>(weapon1));
	sort_inventory();
}

void character::add_to_inventory(const armor& armor1)
{
	if (this->inventory_.size() != 0)
	{
		for (const shared_ptr<item>& e : inventory_)
		{
			if (e->get_name_localization() == armor1.get_name_localization())
			{
				e->increase_number();
				return;
			}
		}
	}
	this->inventory_.push_back(make_shared<armor>(armor1));
	sort_inventory();
}

void character::add_to_inventory(const consumable& consumable1)
{
	if (this->inventory_.size() != 0)
	{
		for (const shared_ptr<item>& e : inventory_)
		{
			if (e->get_name_localization() == consumable1.get_name_localization())
			{
				e->increase_number();
			}
		}
	}
	this->inventory_.push_back(make_shared<consumable>(consumable1));
	sort_inventory();
}

void character::add_to_inventory(const consumable& consumable1, const unsigned int& num)
{
	unsigned int counter = 0;
	bool in_equipment = false;
	do
	{
		if (this->inventory_.size() != 0)
		{
			for (const shared_ptr<item>& e : inventory_)
			{
				if (e->get_name_localization() == consumable1.get_name_localization())
				{
					e->increase_number(consumable1.get_number());
					in_equipment = true;
					break;
				}
			}
		}
		if (!in_equipment) this->inventory_.push_back(make_shared<consumable>(consumable1));
		counter++;
	}
	while (counter != num);
	sort_inventory();
}

void character::add_to_inventory(const spell& item)
{
	this->inventory_.push_back(make_shared<spell>(item));
	sort_inventory();
}

void character::remove_from_inventory()
{
	if (inventory_.size() >= 0)
	{
		bool erased;
		do
		{
			erased = false;
			for (auto it = inventory_.begin(); it != inventory_.end(); ++it)
			{
				if (it->get()->get_number() == 0)
				{
					inventory_.erase(it);
					erased = true;
					break;
				}
			}
		}
		while (erased);
	}
	sort_inventory();
}

void character::equip(const shared_ptr<item>& item)
{
	if (!item) return;

	// weapon ---------------------------------
	if (item->get_type() == item::weapon)
	{
		item->decrease_number();
		this->equip(*dynamic_pointer_cast<weapon>(item));
	}

		// Armor ----------------------------------
	else if (item->get_type() == item::armor)
	{
		item->decrease_number();
		this->equip(*dynamic_pointer_cast<armor>(item));
	}
}

void character::equip(const weapon& item)
{
	sounds.play_sound(sound_manager::weapon, "Equip");

	if (this->weapon_.get_name() != "WepNONE")
	{
		weapon_.increase_number();
		this->add_to_inventory(weapon_);
	}

	this->weapon_ = item;
	refresh_item_dependent();
	remove_from_inventory();
}

void character::equip(const armor& item)
{
	sounds.play_sound(sound_manager::armor, item.get_sfx_dir());

	if (this->armor_.get_name() != "ArmorNONE")
	{
		armor_.increase_number();;
		this->add_to_inventory(armor_);
	}

	this->armor_ = item;
	refresh_item_dependent();
	remove_from_inventory();
}

void character::consume(const shared_ptr<item>& item)
{
	if (!item) return;

	if (item->get_type() == item::consumable)
	{
		item->decrease_number();
		this->consume(*dynamic_pointer_cast<consumable>(item));
	}
}

void character::consume(const consumable& item)
{
	sounds.play_sound(sound_manager::consumable, item.get_sfx_dir());
	if (item.get_effect() == "heal")
	{
		hp_ += item.get_power();
		if (hp_ > hp_max_) hp_ = hp_max_;
	}
	else if (item.get_effect() == "stamina")
	{
		sp_ += item.get_power();
		if (sp_ > sp_max_) sp_ = sp_max_;
	}
	else if (item.get_effect() == "mana")
	{
		mp_ += item.get_power();
		if (mp_ > mp_max_) mp_ = mp_max_;
	}
	remove_from_inventory();
}

void character::sort_inventory()
{
	struct less_than_name
	{
		inline bool operator()(const shared_ptr<item>& item1, const shared_ptr<item>& item2) const
		{
			return (item1->get_name_localization() < item2->get_name_localization());
		}
	};

	sort(inventory_.begin(), inventory_.end(), less_than_name());
}

void character::add_to_active_spells(const spell& spell1)
{
	for (spell& e : active_spells_)
	{
		if (e.get_name_localization() == spell1.get_name_localization())
		{
			e = spell1;
			return;
		}
	}

	active_spells_.push_back(spell1);
}

void character::active_spells_refresh()
{
	for (spell& e : active_spells_)
	{
		if (e.get_span() > 0)
		{
			e.decrease_span();
			this->cast_spell(e);
		}
	}
}

void character::cast_spell(const spell& spell, const shared_ptr<character>& enemy)
{
	if (spell.get_effect() == "heal" || spell.get_effect() == "stamina")
	{
		this->add_to_active_spells(spell);
	}
	else
	{
		enemy->add_to_active_spells(spell);
	}

	int cost = int(spell.get_mana() * ((100.0f + spell.get_requirements() - willpower_) / 100.0f));

	if (cost > spell.get_mana()) cost = spell.get_mana();
	if (cost < spell.get_requirements()) cost = spell.get_requirements();
	mp_ -= cost;

	if (mp_ < 0)
	{
		mp_ = 0;
	}

	sounds.play_sound(sound_manager::spell, spell.get_sfx_dir());
}

void character::cast_spell(const spell& spell)
{
	if (spell.get_effect() == "heal")
	{
		hp_ += spell.get_power();
		if (hp_ > hp_max_) hp_ = hp_max_;
	}
	else if (spell.get_effect() == "stamina")
	{
		sp_ += spell.get_power();
		if (sp_ > sp_max_) sp_ = sp_max_;
	}
	else if (spell.get_effect() == "fire")
	{
		hp_ -= spell.get_power();
		if (hp_ < 0) hp_ = 0;
	}
}

void character::get_spell(const spell& spell)
{
	if (spell.get_effect() == "fire")
	{
		hp_ -= spell.get_power();
		if (hp_ < 0) hp_ = 0;
	}
}

void character::clear_active_spells() noexcept
{
	active_spells_.clear();
}

// Saving|Loading -------------------------
void character::save() const
{
	ofstream out("Saves\\" + current_scenario + "\\" + name_ + ".json");

	// Character info -------------------------
	const json character_info{
		{"location", location_},
		{"subLocation", sub_location_},
		{"level", level_},
		{"exp", exp_},
		{"expNext", exp_next_},
		{"name", name_},
		{
			"stats1", {
				{"hp", hp_},
				{"sp", sp_},
				{"mp", mp_},
				{"hpMax", hp_max_},
				{"spMax", sp_max_},
				{"mpMax", mp_max_},
			}
		},
		{
			"stats2", {
				{"strength", strength_},
				{"endurance", endurance_},
				{"willpower", willpower_},
				{"agility", agility_}
			}
		},
		{"money", money_}
	};
	out << setw(2) << character_info;

	// Inventory info -------------------------
	{
		out << '\n';
		weapon_.save(out);
		out << '\n';
		armor_.save(out);
		files::save(out, inventory_);
	}

	out.close();
}

void character::load(const string& name)
{
	ifstream in("Saves\\" + current_scenario + "\\" + name + ".json");

	// Character info -------------------------
	json character_info;
	in >> character_info;
	{
		location_ = character_info["location"];
		sub_location_ = character_info["subLocation"];
		level_ = character_info["level"];
		exp_ = character_info["exp"];
		exp_next_ = character_info["expNext"];
		name_ = character_info["name"].get<string>();
		hp_ = character_info["stats1"]["hp"];
		sp_ = character_info["stats1"]["sp"];
		mp_ = character_info["stats1"]["mp"];
		hp_max_ = character_info["stats1"]["hpMax"];
		sp_max_ = character_info["stats1"]["spMax"];
		mp_max_ = character_info["stats1"]["mpMax"];
		strength_ = character_info["stats2"]["strength"];
		endurance_ = character_info["stats2"]["endurance"];
		willpower_ = character_info["stats2"]["willpower"];
		agility_ = character_info["stats2"]["agility"];
		money_ = character_info["money"];
	}

	// Inventory info -------------------------
	{
		weapon_.load(in);
		armor_.load(in);
		inventory_ = files::load(in);
	}

	in.close();

	refresh_stat_dependent();
	refresh_item_dependent();
}

// Fighting -------------------------------
void character::increase_hp(const int& inc) noexcept
{
	if (inc > 0)
	{
		hp_ += inc;
		if (hp_ > hp_max_) hp_ = hp_max_;
	}
}

void character::increase_sp(const int& inc) noexcept
{
	if (inc > 0)
	{
		sp_ += inc;
		if (sp_ > sp_max_) sp_ = sp_max_;
	}
}

void character::increase_mp(const int& inc) noexcept
{
	if (inc > 0)
	{
		mp_ += inc;
		if (mp_ > mp_max_) mp_ = mp_max_;
	}
}

void character::get_damaged(const int& dmg) noexcept
{
	if (dmg - defence_ > 0)
	{
		hp_ -= (dmg - defence_);
		if (hp_ < 0) hp_ = 0;
	}
}

void character::decrease_sp(const int& dec) noexcept
{
	if (dec > 0)
	{
		sp_ -= dec;
		if (sp_ < 0) sp_ = 0;
	}
}

void character::decrease_money(const int& dec) noexcept
{
	if (dec > 0)
	{
		money_ -= dec;
		if (money_ <= 0) money_ = 0;
	}
}

void character::increase_def(const int& inc) noexcept
{
	if (inc > 0) defence_ += inc;
}

void character::increase_ev(const int& inc) noexcept
{
	if (inc > 0)
	{
		evasion_chance_ += inc;
	}
}

void character::add_money(const int& inc) noexcept
{
	if (inc > 0)
	{
		money_ += inc;
	}
}
