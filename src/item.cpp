#include "item.hpp"

#include "global_variables.hpp"

using namespace std;

item::item()
{
	type_ = weapon;
	name_ = "NONE";
	requirements_ = 0;
}

item::~item() = default;

void item::increase_number()
{
	number_++;
}

void item::increase_number(const int& inc)
{
	number_ += inc;
}

void item::decrease_number()
{
	number_--;
}

const item::item_type_enum& item::get_type() const
{
	return type_;
}

const string item::get_name() const
{
	return name_;
}

const string item::get_name_localization() const
{
	return item_names[name_];
}

const string& item::get_gfx_dir() const
{
	return gfx_dir_;
}

const string& item::get_sfx_dir() const
{
	return sfx_dir_;
}

const int& item::get_number() const
{
	return number_;
}

const int& item::get_price() const
{
	return price_;
}

const int& item::get_requirements() const
{
	return this->requirements_;
}
