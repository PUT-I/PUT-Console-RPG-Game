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

void item::increase_number() noexcept
{
	number_++;
}

void item::increase_number(const int& inc) noexcept
{
	number_ += inc;
}

void item::decrease_number() noexcept
{
	number_--;
}

const item::item_type_enum& item::get_type() const noexcept
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

const string& item::get_gfx_dir() const noexcept
{
	return gfx_dir_;
}

const string& item::get_sfx_dir() const noexcept
{
	return sfx_dir_;
}

const int& item::get_number() const noexcept
{
	return number_;
}

const int& item::get_price() const noexcept
{
	return price_;
}

const int& item::get_requirements() const noexcept
{
	return this->requirements_;
}
