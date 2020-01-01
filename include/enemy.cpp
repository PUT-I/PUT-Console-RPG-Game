#include "enemy.hpp"

#include "files_util.hpp"
#include "global_variables.hpp"

using namespace std;
using namespace nlohmann;

enemy::enemy()
{
	base_evasion_chance_ = 0;
	base_hit_chance_ = 0;
	name_ = "NONE";
	type_ = "enemy";
	gfx_dir_ = "NONE";
}

// Functions ------------------------------
const vector<string> enemy::load_gfx() const
{
	return files::load_gfx(
		"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Graphics\\Creatures\\" + gfx_dir_ + ".txt"
	);
}

const string& enemy::get_gfx_dir() const
{
	return gfx_dir_;
}

const string& enemy::get_sfx_dir() const
{
	return sfx_dir_;
}

const string enemy::load_name() const
{
	return enemy_names[name_];
}

void enemy::refresh_stat_dependent()
{
	this->dmg_min_ = base_dmg_min_;
	this->dmg_max_ = base_dmg_max_;
	this->defence_ = base_defence_;
	this->hit_chance_ = base_hit_chance_;
	this->evasion_chance_ = base_evasion_chance_;
}

void enemy::load(const json& enemy_info)
{
	type_ = "enemy";

	name_ = enemy_info["name"].get<string>();
	hp_max_ = enemy_info["hpMax"];
	base_dmg_min_ = enemy_info["baseDmgMin"];
	base_dmg_max_ = enemy_info["baseDmgMax"];
	base_defence_ = enemy_info["baseDefence"];
	base_evasion_chance_ = enemy_info["baseEvasionChance"];
	base_hit_chance_ = enemy_info["baseHitChance"];
	exp_ = enemy_info["exp"];
	gfx_dir_ = enemy_info["gfxDir"].get<string>();
	sfx_dir_ = enemy_info["sfxDir"].get<string>();

	hp_ = hp_max_;
	refresh_stat_dependent();
}
