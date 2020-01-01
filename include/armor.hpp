#pragma once

#include "item.hpp"

class armor final : public item
{
private:
	int dmg_protection_;

	// Constructors and a destructor ---------
public:
	armor();

	~armor();

	// Functions ------------------------------
	void save(std::ofstream& out) const override;

	void load(std::ifstream& in) override;

	void load(const json& item_info) override;

	const std::vector<std::string> load_gfx() const override;

	// Accessors ------------------------------
	const int& get_dmg_protection() const noexcept;
};
