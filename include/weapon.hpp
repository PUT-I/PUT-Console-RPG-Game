#pragma once

#include "item.hpp"

class weapon final : public item
{
private:
	int dmg_min_;
	int dmg_max_;

	// Constructors and a destructor ---------
public:
	weapon();

	~weapon();

	// Functions ------------------------------
	void save(std::ofstream& out) const override;

	void load(std::ifstream& in) override;

	void load(const nlohmann::json& item_info) override;

	const std::vector<std::string> load_gfx() const override;

	// Accessors ------------------------------
	const int& get_dmg_min() const;
	const int& get_dmg_max() const;
};
