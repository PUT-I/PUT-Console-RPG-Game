#pragma once

#include <nlohmann/json.hpp>

#include "character.hpp"

class enemy final : public character
{
private:
	// Additional Dependent stats -------------
	int base_evasion_chance_;
	int base_hit_chance_;
	std::string gfx_dir_{};
	std::string sfx_dir_{};

	// Constructors and destructor ------------
public:
	enemy();
	~enemy() = default;

	// Functions ------------------------------
	void refresh_stat_dependent() override;
	const std::vector<std::string> load_gfx() const;
	void load(const nlohmann::json& enemy_info);

	// Accessors ------------------------------
	const std::string& get_gfx_dir() const;
	const std::string& get_sfx_dir() const;
	const std::string load_name() const;
};
