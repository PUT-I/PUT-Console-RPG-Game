#include "weapon.hpp"

#include "exceptions.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"

using namespace std;

weapon::weapon()
{
	type_ = item::weapon;
	name_ = "WepNONE";
	dmg_min_ = 0;
	dmg_max_ = 0;
	requirements_ = 1;
}

weapon::~weapon() = default;

void weapon::save(ofstream& out) const
{
	const json item_info{
		{"type", type_},
		{"name", name_},
		{"gfxDir", gfx_dir_},
		{"sfxDir", sfx_dir_},
		{"price", price_},
		{"number", number_},
		{"requirements", requirements_},
		{"dmgMin", dmg_min_},
		{"dmgMax", dmg_max_}
	};
	out << setw(2) << item_info;
}

void weapon::load(ifstream& in)
{
	json itemInfo;
	in >> itemInfo;
	this->load(itemInfo);
}

void weapon::load(const json& item_info)
{
	if (item_info["type"].get<string>() != "weapon")
	{
		throw resource_exception(
			"Context: Loading weapon item from json."
			"Problem: Item type is not weapon."
		);
	}

	type_ = item::weapon;
	name_ = item_info["name"].get<string>();
	gfx_dir_ = item_info["gfxDir"].get<string>();
	sfx_dir_ = item_info["sfxDir"].get<string>();
	price_ = item_info["price"];
	number_ = item_info["number"];
	requirements_ = item_info["requirements"];
	dmg_min_ = item_info["dmgMin"];
	dmg_max_ = item_info["dmgMax"];
}

const vector<string> weapon::load_gfx() const
{
	return files::load_gfx(
		"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Graphics\\Weapons\\" + gfx_dir_ + ".txt"
	);
}

const int& weapon::get_dmg_min() const noexcept
{
	return this->dmg_min_;
}

const int& weapon::get_dmg_max() const noexcept
{
	return this->dmg_max_;
}
