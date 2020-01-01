#include "spell.hpp"

#include <fstream>
#include <iomanip>

#include "exceptions.hpp"
#include "files_util.hpp"
#include "global_variables.hpp"

using namespace std;
using namespace nlohmann;

spell::spell()
{
	type_ = item::spell;
	effect_ = "NONE";
	power_ = 0;
	requirements_ = 1;
	span_ = 0;
	mana_ = 0;
}

spell::~spell() = default;

void spell::save(ofstream& out) const
{
	const json item_info{
		{"type", type_},
		{"name", name_},
		{"gfxDir", gfx_dir_},
		{"sfxDir", sfx_dir_},
		{"price", price_},
		{"number", number_},
		{"requirements", requirements_},
		{"effect", effect_},
		{"power", power_},
		{"mana", mana_},
		{"span", span_}
	};
	out << setw(2) << item_info;
}

void spell::load(ifstream& in)
{
	json item_info;
	in >> item_info;
	this->load(item_info);
}

void spell::load(const json& item_info)
{
	if (item_info["type"].get<string>() != "spell")
	{
		throw resource_exception(
			"Context: Loading spell item from json."
			"Problem: Item type is not spell."
		);
	}

	type_ = item::spell;
	name_ = item_info["name"].get<string>();
	gfx_dir_ = item_info["gfxDir"].get<string>();
	sfx_dir_ = item_info["sfxDir"].get<string>();
	price_ = item_info["price"];
	number_ = item_info["number"];
	requirements_ = item_info["requirements"];
	effect_ = item_info["effect"].get<string>();
	power_ = item_info["power"];
	mana_ = item_info["mana"];
	span_ = item_info["span"];
}

const vector<string> spell::load_gfx() const
{
	return files::load_gfx(
		"GameFiles\\Scenarios\\" + current_scenario + "\\Resources\\Graphics\\Spells\\" + gfx_dir_ + ".txt");
}

void spell::decrease_span()
{
	span_--;
}

const string& spell::get_effect() const
{
	return this->effect_;
}

const int& spell::get_power() const
{
	return this->power_;
}

const int& spell::get_mana() const
{
	return this->mana_;
}

const int& spell::get_span() const
{
	return this->span_;
}
