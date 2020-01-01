#pragma once

#include "armor.hpp"
#include "color_util.hpp"
#include "consumable.hpp"
#include "enemy.hpp"
#include "global_headers.h"
#include "sound_manager.hpp"
#include "spell.hpp"
#include "weapon.hpp"

extern std::string current_scenario;

// Text maps ------------------------------
extern std::map<std::string, std::string> item_names;
extern std::map<std::string, std::string> enemy_names;
extern std::map<std::string, std::string> s_line_texts; // Single line texts
extern std::map<std::string, std::vector<std::string>> m_line_texts; // Multiple line texts

// Graphics map ---------------------------
extern std::map<std::string, std::vector<std::string>> graphics;

// Enemies --------------------------------
extern std::map<std::string, enemy> enemies;

// Item maps ------------------------------
namespace items
{
	extern weapon wep_none;
	extern armor armor_none;
	extern std::map<std::string, weapon> weapons;
	extern std::map<std::string, armor> armors;
	extern std::map<std::string, consumable> consumables;
	extern std::map<std::string, spell> spells;
}

extern color_util color;

extern int global_font_size;
extern int story_font_inc;
extern std::string language;
extern std::string code_page;

extern const std::vector<std::string> lines;
extern const std::vector<std::string> location_text_blank;
extern const std::vector<std::string> location_options_blank;
extern const std::string equal_signs;

extern sound_manager sounds;
