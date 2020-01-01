#pragma once

#include <nlohmann/json.hpp>
#include <string>

class item
{
	// Enums
public:
	enum item_type_enum
	{
		weapon,
		armor,
		spell,
		consumable
	};
	
protected:
	item_type_enum type_;
	std::string name_;
	std::string gfx_dir_;
	std::string sfx_dir_;
	int price_ = 0;
	int number_ = 1;
	int requirements_;

public:
	item();

	virtual ~item();

	// Functions ------------------------------
	virtual void save(std::ofstream& out) const = 0;
	virtual void load(std::ifstream& in) = 0;
	virtual void load(const nlohmann::json& item_info) = 0;
	virtual const std::vector<std::string> load_gfx() const = 0;

	// Modifiers ------------------------------
	void increase_number();
	void increase_number(const int& inc);
	void decrease_number();

	// Accessors ------------------------------
	const item_type_enum& get_type() const;
	const std::string get_name() const;;
	const std::string get_name_localization() const;
	const std::string& get_gfx_dir() const;
	const std::string& get_sfx_dir() const;
	const int& get_number() const;
	const int& get_price() const;
	const int& get_requirements() const;
};
