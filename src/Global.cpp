#include "Global.hpp"

using namespace std;

string currentScenario;

//Text maps ------------------------------
map<string, string> itemNames;
map<string, string> enemyNames;
map<string, string> sLineTexts;	//Single line texts
map<string, vector<string>> mLineTexts;	//Multiple line texts

//Graphics map ------------------------------
map<string, vector<string>> graphics;