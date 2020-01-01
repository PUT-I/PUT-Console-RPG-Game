#include "consumable.hpp"

#include "exceptions.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"

using namespace std;

consumable::consumable()
{
	type_ = item::consumable;
	effect_ = "NONE";
	power_ = 0;
	requirements_ = 1;
}

consumable::~consumable() = default;

void consumable::save(ofstream& out) const
{
	const json item_info{
		{"type", type_},
		{"name", name_},
		{"gfxDir", gfx_dir_},
		{"sfxDir", sfx_dir_},
		{"price", price_},
		{"number", number_},
		{"effect", effect_},
		{"power", power_}
	};
	out << setw(2) << item_info;
}

void consumable::load(ifstream& in)
{
	json item_info;
	in >> item_info;
	this->load(item_info);
}

void consumable::load(const json& item_info)
{
	if (item_info["type"].get<string>() != "consumable")
	{
		throw resource_exception(
			"Context: Loading consumable item from json."
			"Problem: Item type is not consumable."
		);
	}
	
	type_ = item::consumable;
	name_ = item_info["name"].get<string>();
	gfx_dir_ = item_info["gfxDir"].get<string>();
	sfx_dir_ = item_info["sfxDir"].get<string>();
	price_ = item_info["price"];
	number_ = item_info["number"];
	effect_ = item_info["effect"].get<string>();
	power_ = item_info["power"];
}

const vector<string> consumable::load_gfx() const
{
	return files::load_gfx(
		"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Graphics\\Consumables\\" + gfx_dir_ + ".txt");
}

const string& consumable::get_effect() const noexcept
{
	return this->effect_;
}

const int& consumable::get_power() const noexcept
{
	return this->power_;
}
