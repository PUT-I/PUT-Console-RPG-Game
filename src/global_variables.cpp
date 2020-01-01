#include "global_variables.hpp"

using namespace std;

string current_scenario;

// Text maps ------------------------------
map<string, string> item_names;
map<string, string> enemy_names;
map<string, string> s_line_texts; // Single line texts
map<string, vector<string>> m_line_texts; // Multiple line texts

// Graphics map ------------------------------
map<string, vector<string>> graphics;

//----------------------- Enemies -----------------------//
map<string, enemy> enemies;

// Itemp Maps -----------------------------
namespace items
{
	weapon wep_none;
	armor armor_none;
	map<string, weapon> weapons;
	map<string, armor> armors;
	map<string, consumable> consumables;
	map<string, spell> spells;
}

color_util color;

int global_font_size;
int story_font_inc;
string language;
string code_page;

const vector<string> lines{ 20, "|" };
const vector<string> location_text_blank{ 20, string(51, ' ') };
const vector<string> location_options_blank{ 5, string(81, ' ') };
const string equal_signs{ string(81, '=') };

sound_manager sounds;
