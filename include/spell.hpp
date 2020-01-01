#pragma once

#include "item.hpp"

class spell final : public item
{
private:
	std::string effect_{};
	int power_;
	int mana_;
	int span_;

	// Constructors and a destructor ---------
public:
	spell();

	~spell();

	// Functions ------------------------------
	void save(std::ofstream& out) const override;

	void load(std::ifstream& in) override;

	void load(const nlohmann::json& item_info) override;

	const std::vector<std::string> load_gfx() const override;

	// Modifiers ------------------------------
	void decrease_span();

	// Accessors ------------------------------
	const std::string& get_effect() const;
	const int& get_power() const;
	const int& get_mana() const;
	const int& get_span() const;
};
