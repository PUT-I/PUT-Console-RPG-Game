#include "armor.hpp"

#include "exceptions.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"

using namespace std;

armor::armor()
{
	type_ = item::armor;
	name_ = "NONE";
	dmg_protection_ = 0;
}

armor::~armor() = default;

void armor::save(ofstream& out) const
{
	const json item_info{
		{"type", type_},
		{"name", name_},
		{"gfxDir", gfx_dir_},
		{"sfxDir", sfx_dir_},
		{"price", price_},
		{"number", number_},
		{"requirements", requirements_},
		{"dmgProtection", dmg_protection_}
	};
	out << setw(2) << item_info;
}

void armor::load(ifstream& in)
{
	json item_info;
	in >> item_info;
	this->load(item_info);
}

void armor::load(const json& item_info)
{
	if (item_info["type"].get<string>() != "armor")
	{
		throw resource_exception(
			"Context: Loading armor item from json."
			"Problem: Item type is not armor."
		);
	}
	
	type_ = item::armor;
	name_ = item_info["name"].get<string>();
	gfx_dir_ = item_info["gfxDir"].get<string>();
	sfx_dir_ = item_info["sfxDir"].get<string>();
	price_ = item_info["price"];
	number_ = item_info["number"];
	requirements_ = item_info["requirements"];
	dmg_protection_ = item_info["dmgProtection"];
}

const vector<string> armor::load_gfx() const
{
	return files::load_gfx(
		"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Graphics\\Armors\\" + gfx_dir_ + ".txt");
}

const int& armor::get_dmg_protection() const noexcept
{
	return this->dmg_protection_;
}
