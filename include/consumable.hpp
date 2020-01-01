#pragma once

#include "item.hpp"

class consumable final : public item
{
private:
	std::string effect_{};
	int power_;

	// Constructors and a destructor ---------
public:
	consumable();

	~consumable();

	// Functions ------------------------------
	void save(std::ofstream& out) const override;

	void load(std::ifstream& in) override;

	void load(const nlohmann::json& item_info) override;

	const std::vector<std::string> load_gfx() const override;

	// Accessors ------------------------------
	const std::string& get_effect() const;
	const int& get_power() const;
};
