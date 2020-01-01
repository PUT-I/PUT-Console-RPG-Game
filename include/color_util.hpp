#pragma once

#include "global_headers.h"

class color_util
{
private:
	bool greyscale_ = false;
	const std::string red_{0x1b, '[', '1', ';', '3', '1', 'm'};
	const std::string green_{0x1b, '[', '1', ';', '9', '2', 'm'};
	const std::string blue_{0x1b, '[', '1', ';', '3', '4', 'm'};
	const std::string yellow_{0x1b, '[', '1', ';', '3', '3', 'm'};
	const std::string grey_{0x1b, '[', '1', ';', '3', '0', 'm'};
	const std::string normal_{0x1b, '[', '0', ';', '3', '9', 'm'};

	// Constructors and a destructor ---------
public:
	color_util();
	~color_util();

	// Modifiers ------------------------------
	void set_greyscale(const bool& yes_no) noexcept;

	// Accessors ------------------------------
	const std::string& get(const std::string& choice) const noexcept;

	const bool& get_greyscale() const noexcept;
};
