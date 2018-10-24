#include "Character.hpp"

using namespace std;

map<string, Enemy> enemies;

namespace files {
	void loadGraphics() {
		graphics.clear();

		string temp;

		for (auto it = enemies.begin(); it != enemies.end(); it++) {
			graphics[it->second.getGfxDir()] = it->second.loadGfx();
		}
		for (auto it = itms::weapons.begin(); it != itms::weapons.end(); it++) {
			graphics[it->second.getGfxDir()] = it->second.loadGfx();
		}
		for (auto it = itms::armors.begin(); it != itms::armors.end(); it++) {
			graphics[it->second.getGfxDir()] = it->second.loadGfx();
		}
		for (auto it = itms::spells.begin(); it != itms::spells.end(); it++) {
			graphics[it->second.getGfxDir()] = it->second.loadGfx();
		}
		for (auto it = itms::consumables.begin(); it != itms::consumables.end(); it++) {
			graphics[it->second.getGfxDir()] = it->second.loadGfx();
		}
		for (auto &p : filesystem::directory_iterator("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Locations")) {
			std::ifstream in(p);
			temp = p.path().filename().string();
			temp.erase(temp.size() - 4, 4);
			graphics[temp] = loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Locations\\" + p.path().filename().string());
			in.close();
		}
	}
}

Character::Character() noexcept {
	type = "character";

	//Location -------------------------------
	this->location    = 0;
	this->subLocation = 0;

	//Levelling ------------------------------
	{
		this->level = 0;
		this->exp = 0;
		this->expNext = 0;
	}

	this->name = "NONE";
	//Stats1 ---------------------------------
	{
		this->hp = 0;
		this->sp = 0;
		this->mp = 0;
		this->hpMax = 0;
		this->spMax = 0;
		this->mpMax = 0;
	}

	//Dependent stats ------------------------
	{
		this->baseDmgMin = 0;
		this->baseDmgMax = 0;
		this->dmgMin = 0;
		this->dmgMax = 0;
		this->baseDefence = 0;
		this->evasionChance = 0;
		this->hitChance = 0;
		this->baseDefence = 0;
		this->defence = 0;
	}

	//Stats2 ---------------------------------
	{
		this->strength = 0;
		this->endurance = 0;
		this->willpower = 0;
		this->agility = 0;
	}

	this->stat1Points = 0;
	this->stat2Points = 0;

	this->weapon;
	this->armor;
}

//Functions ------------------------------
void Character::clear() {
	inventory.clear();
	activeSpells.clear();
	weapon = Weapon();
	armor = Armor();
}
void Character::refreshItemDependent() noexcept {
	this->dmgMin = baseDmgMin + weapon.getDmgMin();
	this->dmgMax = baseDmgMax + weapon.getDmgMax();
	this->defence = baseDefence + armor.getDmgProtection();
}
void Character::refreshStatDependent() noexcept {
	this->baseDmgMin = strength;
	this->baseDmgMax = strength * 2;
	this->hitChance = 48 + agility * 2;
	this->evasionChance = agility;
	this->baseDefence = endurance * 2;
}

void Character::stats1Setup() {
	//Declarations ----------------------------
	unsigned int choice = 0;
	const int stat1PointsPrev = stat1Points;
	int result;

	vector<int*> stats{ &hpMax, &spMax, &mpMax };
	const vector<int> statsPrev{ hpMax, spMax, mpMax };	//Used to prevent decreasing stat below its prev number

	//Stat Names and Setting Colors -----------
	string Health{ color.get("red") };
	string Stamina{ color.get("green") };
	string Mana{ color.get("blue") };

	//Adding Option Names ---------------------
	{
		Health	+= mLineTexts["Stats1Names"][0];
		Stamina += mLineTexts["Stats1Names"][1];
		Mana	+= mLineTexts["Stats1Names"][2];
	}

	const vector<string> options{ Health, Stamina, Mana, sLineTexts["Continue"] };

	//Interface ------------------------------
	ClearConsole();
	PrintBox(0, 0, 54, 17);
	PrintText(27 - sLineTexts["StatsTitle"].size() / 2, 0, sLineTexts["StatsTitle"]);
	do {
		if (choice >= 0 && choice < options.size()) {
			refreshStatDependent();
			
			CursorPos(8, 2);
			cout << sLineTexts["AvailablePoints"] << stat1Points << "    ";
			CursorPos(0, 4);
			displayOptionsNoNum(choice, options, stats);
		}
		choiceLimit(choice, options.size());
		result = upDownLeftRightControls(choice, options.size(), stat1Points, stat1PointsPrev, stats, statsPrev, 10);
		if (result == -1) {
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			PrintText(8, 4, sLineTexts["SpendAllPoints"]);

			CursorPos(8, 6);
			SpacePause();
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			PrintText(27 - sLineTexts["StatsTitle"].size() / 2, 0, sLineTexts["StatsTitle"]);
		}
	} while (result != 1);
	audio.enter();

	this->hp = this->hpMax;
	this->sp = this->spMax;
	this->mp = this->mpMax;
}
void Character::stats2Setup() {
	//Declarations ----------------------------
	unsigned int choice = 0;
	int result;
	const int stat2PointsPrev = stat2Points;
	vector<int*> stats{ &strength, &endurance, &willpower, &agility };
	const vector<int> statsPrev{ strength, endurance, willpower, agility };	//Used to prevent decreasing stat below its prev number

	vector<string> options = mLineTexts["Stats2Names"];
	options.push_back(sLineTexts["Continue"]);

	//Interface ------------------------------
	ClearConsole();
	PrintBox(0, 0, 54, 17);
	PrintText(27 - sLineTexts["StatsTitle"].size() / 2, 0, sLineTexts["StatsTitle"]);
	do {

		if (choice >= 0 && choice < options.size()) {
			refreshStatDependent();
			CursorPos(8, 2);
			cout << sLineTexts["AvailablePoints"] << stat2Points << "    ";
			CursorPos(0, 4);
			displayDependent();
			CursorMove(0, 2);
			displayOptionsNoNum(choice, options, stats);
		}

		choiceLimit(choice, options.size());
		result = upDownLeftRightControls(choice, options.size(), stat2Points, stat2PointsPrev, stats, statsPrev, 1);
		if (result == -1) {
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			PrintText(8, 4, sLineTexts["SpendAllPoints"]);

			CursorPos(8, 6);
			SpacePause();
			ClearConsole();
			PrintBox(0, 0, 54, 17);
			PrintText(27 - sLineTexts["StatsTitle"].size() / 2, 0, sLineTexts["StatsTitle"]);
		}
	} while (result != 1);
}
void Character::initialize(const string &name_){
	type = "character";

	//Location -------------------------------
	this->location = 1;
	this->subLocation = 1;

	//Levelling ------------------------------
	{
		this->level = 1;
		this->exp = 0;
		this->expNext = 100;
	}

	this->name = name_;
	//Stats1 ---------------------------------
	{
		this->hpMax = 100;
		this->spMax = 100;
		this->mpMax = 100;
	}

	//Stats2 ---------------------------------
	{
		this->strength = 1;
		this->endurance = 1;
		this->willpower = 1;
		this->agility = 1;
	}

	this->stat1Points = 3;
	this->stat2Points = 10;

	stats1Setup();
	stats2Setup();

	this->weapon = itms::weapons["NONE"];
	this->armor  = itms::armors["Clothes"];
	this->armor.decreaseNumber();

	//Refreshing -----------------------------
	{
		refreshStatDependent();
		refreshItemDependent();
	}
}

void Character::display() const {
	const unsigned int y1 = 0;
	const unsigned int x1 = 1;
	const unsigned int x2 = 31;

	const vector<unsigned int> y{
		y1+6, y1+3, y1+11, y1+6, y1+12
	};

	//Name and Levelling ---------------------
	{
		CursorPos(x1, y[0]);
		cout << mLineTexts["SheetNandL"][0] << name;
		CursorPos(x1, y[0] + 1);
		cout << mLineTexts["SheetNandL"][1] << level;
		CursorPos(x1, y[0] + 2);
		cout << mLineTexts["SheetNandL"][2] << exp << "/" << expNext;
		CursorPos(x1, y[0] + 3);
		cout << color.get("yellow") << mLineTexts["SheetNandL"][3] << money << color.get("normal");

		PrintBox(x1 - 1, y[0]-1, 29, 4);
	}

	//Equipment ------------------------------
	{
		CursorPos(x1, y[1]);
		cout << mLineTexts["SheetEquipement"][0] << weapon.getName();
		CursorPos(x1, y[1] + 1);
		cout << mLineTexts["SheetEquipement"][1] << armor.getName();
		
		PrintBox(x1 - 1, y[1] - 1, 59, 2);
	}

	//Stats1 ---------------------------------
	{
		CursorPos(x1, y[2]);
		cout << color.get("red")   << mLineTexts["Stats1Names"][0] << hp << "/" << hpMax << color.get("normal");
		CursorPos(x1, y[2] + 1);
		cout << color.get("green") << mLineTexts["Stats1Names"][1] << sp << "/" << spMax << color.get("normal");
		CursorPos(x1, y[2] + 2);
		cout << color.get("blue")  << mLineTexts["Stats1Names"][2] << mp << "/" << mpMax << color.get("normal");

		PrintBox(x1 - 1, y[2] - 1, 29, 3);
	}

	//Dependent Stats ------------------------
	{
		CursorPos(x2, y[3]);
		cout << mLineTexts["StatsDepNames"][0] << dmgMin;
		CursorPos(x2, y[3] + 1);
		cout << mLineTexts["StatsDepNames"][1] << dmgMax;
		CursorPos(x2, y[3] + 2);
		cout << mLineTexts["StatsDepNames"][2] << defence;
		CursorPos(x2, y[3] + 3);
		cout << mLineTexts["StatsDepNames"][3] << hitChance;
		CursorPos(x2, y[3] + 4);
		cout << mLineTexts["StatsDepNames"][4] << evasionChance;

		PrintBox(x2 - 1, y[3] - 1, 29, 5);
	}

	//Stats2 ---------------------------------
	{
		CursorPos(x2, y[4]);
		cout << mLineTexts["Stats2Names"][0] << strength;
		CursorPos(x2, y[4] + 1);
		cout << mLineTexts["Stats2Names"][1] << endurance;
		CursorPos(x2, y[4] + 2);
		cout << mLineTexts["Stats2Names"][2] << willpower;
		CursorPos(x2, y[4] + 3);
		cout << mLineTexts["Stats2Names"][3] << agility;

		PrintBox(x2 - 1, y[4] - 1, 29, 4);
	}

	//Title ----------------------------------
	{
		PrintText(30 - sLineTexts["SheetTitle"].size() / 2, y[1]-1, sLineTexts["SheetTitle"]);
	}

	//Space Pause Position -------------------
	CursorPos(1, y[2] + 4);
}
void Character::displayDependent() const {
	const int cursorPrevY = GetCursorPos().Y;

	CursorPos(8, cursorPrevY);
	cout << mLineTexts["StatsDepNames"][0] << baseDmgMin    << "  ";
	CursorPos(8, cursorPrevY + 1);
	cout << mLineTexts["StatsDepNames"][1] << baseDmgMax    << "  ";
	CursorPos(8, cursorPrevY + 2);
	cout << mLineTexts["StatsDepNames"][2] << baseDefence   << "  ";
	CursorPos(8, cursorPrevY + 3);
	cout << mLineTexts["StatsDepNames"][3] << hitChance     << "  ";
	CursorPos(8, cursorPrevY + 4);
	cout << mLineTexts["StatsDepNames"][4] << evasionChance << "  ";
}

void Character::addExp(const int& exp_) {
	const string fullMsg{ name + mLineTexts["MsgGain"][0] + color.get("yellow") + to_string(exp_) + mLineTexts["MsgGain"][1] + color.get("normal") };

	ClearConsole();
	PrintBox(0, 0, 54, 17);
	CursorPos(8, 4);
	cout << fullMsg;
	exp += exp_;
	if (exp >= expNext) {
		audio.misc("LevelUp");
		levelUp();
	}
	else {
		CursorPos(8, 6);
		SpacePause();
	}
	ClearConsole();
}
void Character::levelUp(){
	//Text -----------------------------------
	{
		CursorPos(8, 6);
		cout << sLineTexts["MsgLevelUp"];
		CursorPos(8, 8);
		SpacePause();
	}

	while (exp >= expNext) {
		this->level++;

		//Points increasing ----------------------
		{
			if (level < 10) {
				this->stat1Points += 3;
				this->stat2Points += 2;
			}
			else if (10 <= level && level < 20) {
				this->stat1Points += 5;
				this->stat2Points += 3;
			}
			else {
				this->stat1Points += 10;
				this->stat2Points += 5;
			}
		}

		this->exp -= expNext;
		this->expNext += 100;
	}
	stats1Setup();
	stats2Setup();
}

//Modifiers ------------------------------
void Character::addToInventory(const shared_ptr<Item> &item){
	if (!item) return;
	
	if (inventory.size() != 0) {
		for (const shared_ptr<Item> &e : inventory) {
			if (e->getName() == item->getName()) {
				e->increaseNumber();
				return;
			}
		}
	}

	this->inventory.push_back(item);
	sortInventory();
}
void Character::addToInventory(const Weapon &item) {
	if (inventory.size() != 0) {
		for (const shared_ptr<Item> &e : inventory) {
			if (e->getName() == item.getName()) {
				e->increaseNumber();
				return;
			}
		}
	}
	this->inventory.push_back(make_shared<Weapon>(item));
	sortInventory();
}
void Character::addToInventory(const Armor &item) {
	if (this->inventory.size() != 0) {
		for (const shared_ptr<Item> &e : inventory) {
			if (e->getName() == item.getName()) {
				e->increaseNumber();
				return;
			}
		}
	}
	this->inventory.push_back(make_shared<Armor>(item));
	sortInventory();
}
void Character::addToInventory(const Consumable &item) {
	if (this->inventory.size() != 0) {
		for (const shared_ptr<Item> &e : inventory) {
			if (e->getName() == item.getName()) {
				e->increaseNumber();
			}
		}
	}
	this->inventory.push_back(make_shared<Consumable>(item));
	sortInventory();
}
void Character::addToInventory(const Consumable &item, const unsigned int &num) {
	unsigned int counter = 0;
	bool inEquipment = false;
	do {
		if (this->inventory.size() != 0) {
			for (const shared_ptr<Item> &e : inventory) {
				if (e->getName() == item.getName()) {
					e->increaseNumber(item.getNumber());
					inEquipment = true;
					break;
				}
			}
		}
		if (!inEquipment) this->inventory.push_back(make_shared<Consumable>(item));
		counter++;
	} while (counter != num);
	sortInventory();
}
void Character::addToInventory(const Spell &item) {
	this->inventory.push_back(make_shared<Spell>(item));
	sortInventory();
}
void Character::removeFromInventory() {
	if (inventory.size() >= 0) {
		bool erased;
		do {
			erased = false;
			for (auto it = inventory.begin(); it != inventory.end(); it++) {
				if (it->get()->getNumber() == 0) {
					inventory.erase(it);
					erased = true;
					break;
				}
			}
		} while (erased);
	}
	sortInventory();
}

void Character::equip(const shared_ptr<Item> &item) {
	if (!item) return;

	//Weapon ---------------------------------
	if (item->getType() == "weapon") {
		item->decreaseNumber();
		this->equip(*dynamic_pointer_cast<Weapon>(item));
	}

	//Armor ----------------------------------
	else if (item->getType() == "armor") {
		item->decreaseNumber();
		this->equip(*dynamic_pointer_cast<Armor>(item));
	}
}
void Character::equip(const Weapon &item) {
	audio.weapon("Equip");

	if (this->weapon.getNameRaw() != "WepNONE") {
		weapon.increaseNumber();
		this->addToInventory(weapon);
	}

	this->weapon = item;
	refreshItemDependent();
	removeFromInventory();
}
void Character::equip(const Armor &item) {
	audio.armor(item.getSfxDir());

	if (this->armor.getNameRaw() != "ArmorNONE") {
		armor.increaseNumber();
		;		this->addToInventory(armor);
	}

	this->armor = item;
	refreshItemDependent();
	removeFromInventory();
}

void Character::consume(const shared_ptr<Item> &item){
	if (!item) return;

	if (item->getType() == "consumable") {
		item->decreaseNumber();
		this->consume(*dynamic_pointer_cast<Consumable>(item));
	}
}
void Character::consume(const Consumable &item){
	audio.consumable(item.getSfxDir());
	if (item.getEffect() == "heal") {
		hp += item.getPower();
		if (hp > hpMax) hp = hpMax;
	}
	else if (item.getEffect() == "stamina") {
		sp += item.getPower();
		if (sp > spMax) sp = spMax;
	}
	else if (item.getEffect() == "mana") {
		mp += item.getPower();
		if (mp > mpMax) mp = mpMax;
	}
	removeFromInventory();
}

void Character::sortInventory() {
	struct less_than_name
	{
		inline bool operator() (const shared_ptr<Item> &item1, const shared_ptr<Item> &item2)
		{
			return (item1->getName() < item2->getName());
		}
	};

	sort(inventory.begin(), inventory.end(), less_than_name());
}

void Character::addToActiveSpells(Spell spell) {
	for (Spell& e : activeSpells) {
		if (e.getName() == spell.getName()) {
			e = spell;
			return;
		}
	}
	
	activeSpells.push_back(spell);
}
void Character::activeSpellsRefresh() {
	for (Spell &e : activeSpells) {
		if (e.getSpan() > 0) {
			e.decreaseSpan();
			this->castSpell(e);
		}
	}
}
void Character::castSpell(const Spell &spell, const shared_ptr<Character> &enemy) {
	if (spell.getEffect() == "heal" || spell.getEffect() == "stamina") {
		this->addToActiveSpells(spell);
	}
	else {
		enemy->addToActiveSpells(spell);
	}

	int cost = int( spell.getMana() * ((100.0f + spell.getRequirements() - willpower) / 100.0f) );

	if (cost > spell.getMana()) cost = spell.getMana();
	if (cost < spell.getRequirements()) cost = spell.getRequirements();
	mp -= cost;

	if (mp < 0) {
		mp = 0;
	}

	audio.spell(spell.getSfxDir());
}
void Character::castSpell(const Spell &spell) {
	if (spell.getEffect() == "heal") {
		hp += spell.getPower();
		if (hp > hpMax) hp = hpMax;
	}
	else if (spell.getEffect() == "stamina") {
		sp += spell.getPower();
		if (sp > spMax) sp = spMax;
	}
	else if(spell.getEffect() == "fire") {
		hp -= spell.getPower();
		if (hp < 0) hp = 0;
	}
}
void Character::getSpell(const Spell &spell) {
	if (spell.getEffect() == "fire") {
		hp -= spell.getPower();
		if (hp < 0) hp = 0;
	}
}
void Character::clearActiveSpells() noexcept {
	activeSpells.clear();
}

//Saving|Loading -------------------------
void Character::save() const {
	ofstream out("Saves\\" + currentScenario + "\\" + name + ".json");

	//Character info -------------------------
	json characterInfo{
		{"location", location},
		{"subLocation", subLocation},
		{"level", level},
		{"exp", exp},
		{"expNext", expNext},
		{"name", name},
		{"stats1", {
			{"hp", hp},
			{"sp", sp},
			{"mp", mp},
			{ "hpMax", hpMax },
			{ "spMax", spMax },
			{ "mpMax", mpMax },
		}},		
		{ "stats2", {
			{ "strength", strength },
			{ "endurance", endurance },
			{ "willpower", willpower },
			{ "agility", agility }
		}},
		{"money", money}
		};
		out << setw(2) << characterInfo;

	//Inventory info -------------------------
	{
		out << '\n';
		weapon.save(out);
		out << '\n';
		armor.save(out);
		files::save(out, inventory);
	}

	out.close();
}
void Character::load(const string &name_){
	ifstream in("Saves\\" + name_ + ".json");

	//Character info -------------------------
	json characterInfo;
	in >> characterInfo;
	{
		location = characterInfo["location"];
		subLocation = characterInfo["subLocation"];
		level = characterInfo["level"];
		exp = characterInfo["exp"];
		expNext = characterInfo["expNext"];
		name = characterInfo["name"].get<string>();
		hp = characterInfo["stats1"]["hp"];
		sp = characterInfo["stats1"]["sp"];
		mp = characterInfo["stats1"]["mp"];
		hpMax = characterInfo["stats1"]["hpMax"];
		spMax = characterInfo["stats1"]["spMax"];
		mpMax = characterInfo["stats1"]["mpMax"];
		strength  = characterInfo["stats2"]["strength"];
		endurance = characterInfo["stats2"]["endurance"];
		willpower = characterInfo["stats2"]["willpower"];
		agility = characterInfo["stats2"]["agility"];
		money = characterInfo["money"];
	}

	//Inventory info -------------------------
	{
		weapon.load(in);
		armor.load(in);
		inventory = files::load(in);
	}

	in.close();

	refreshStatDependent();
	refreshItemDependent();
}

//Fighting -------------------------------
void Character::addHp(const int &inc) noexcept {
	if (inc > 0) {
		hp += inc;
		if (hp > hpMax) hp = hpMax;
	}
}
void Character::addSp(const int &inc) noexcept {
	if (inc > 0) {
		sp += inc;
		if (sp > spMax) sp = spMax;
	}
}
void Character::addMp(const int &inc) noexcept {
	if (inc > 0) {
		mp += inc;
		if (mp > mpMax) mp = mpMax;
	}
}
void Character::getDamaged(const int &dmg) noexcept {
	if (dmg - defence > 0) {
		hp -= (dmg - defence);
		if (hp < 0) hp = 0;
	}
}
void Character::decreaseSp(const int &dec) noexcept {
	if (dec > 0) {
		sp -= dec;
		if (sp < 0) sp = 0;
	}
}
void Character::decreaseMoney(const int & dec) noexcept {
	if (dec > 0) {
		money -= dec;
		if (money <= 0) money = 0;
	}
}
void Character::increaseDef(const int & inc) noexcept {
	if(inc > 0) defence += inc;
}
void Character::increaseEv(const int & inc) noexcept {
	if (inc > 0) {
		evasionChance += inc;
	}
}
void Character::addMoney(const int & inc) noexcept {
	if (inc > 0) {
		money += inc;
	}
}

/*-------------------------------- Enemy --------------------------------*/
//Functions ------------------------------
void Enemy::refreshStatDependent() noexcept {
	this->dmgMin = baseDmgMin;
	this->dmgMax = baseDmgMax;
	this->defence = baseDefence;
	this->hitChance = baseHitChance;
	this->evasionChance = baseEvasionChance;
}
void Enemy::load(const json &enemyInfo) {
		type = "enemy";

		name = enemyInfo["name"].get<string>();
		hpMax = enemyInfo["hpMax"];
		baseDmgMin = enemyInfo["baseDmgMin"];
		baseDmgMax = enemyInfo["baseDmgMax"];
		baseDefence = enemyInfo["baseDefence"];
		baseEvasionChance = enemyInfo["baseEvasionChance"];
		baseHitChance = enemyInfo["baseHitChance"];
		exp = enemyInfo["exp"];
		gfxDir = enemyInfo["gfxDir"].get<string>();
		sfxDir = enemyInfo["sfxDir"].get<string>();

		hp = hpMax;
		refreshStatDependent();
}