#include "color_util.hpp"

color_util::color_util() = default;
color_util::~color_util() = default;

void color_util::set_greyscale(const bool& yes_no) noexcept
{
	greyscale_ = yes_no;
}

const std::string& color_util::get(const std::string& choice) const noexcept
{
	if (choice == "grey" || choice == "5") return grey_;
	else if (choice == "normal" || choice == "0" || greyscale_ == true) return normal_;
	else if (choice == "red" || choice == "1") return red_;
	else if (choice == "green" || choice == "2") return green_;
	else if (choice == "blue" || choice == "3") return blue_;
	else if (choice == "yellow" || choice == "4") return yellow_;
	else return normal_;
}

const bool& color_util::get_greyscale() const noexcept
{
	return greyscale_;
}
