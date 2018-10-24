#pragma once

#include "FrequentHeaders.h"

extern std::string currentScenario;

//Text maps ------------------------------
extern std::map<std::string, std::string> itemNames, enemyNames;
extern std::map<std::string, std::string> sLineTexts;	//Single line texts
extern std::map<std::string, std::vector<std::string>> mLineTexts;	//Multiple line texts

//Graphics map ------------------------------
extern std::map<std::string, std::vector<std::string>> graphics;