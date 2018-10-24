#pragma once

#include "FrequentHeaders.h"
#include "Files.hpp"

static const int zero = 0;
static const std::string emptyString{};

class Item {
protected:
	std::string type;
	std::string name;
	std::string gfxDir;
	std::string sfxDir;
	int price = 0;
	int number = 1;
	int requirements;

	//Construnctors and a destructor ---------
public:
	Item() noexcept {
		type = "NONE";
		name = "NONE";
		requirements = 0;
	}
	virtual ~Item() {}

	//Functions ------------------------------
	virtual void save(std::ofstream &out) const {}
	virtual void load(std::ifstream &in) {}
	virtual void load(const json &itemInfo) {}
	virtual const std::vector<std::string> loadGfx() const {
		std::vector<std::string> NONE{};
		return NONE;
	}

	//Modifiers ------------------------------
	void increaseNumber() noexcept { number++; }
	void increaseNumber(const unsigned int &inc) noexcept { number +=inc; }
	void decreaseNumber() noexcept { number--; }

	//Accessors ------------------------------
	const std::string& getType() const noexcept { return type; }
	const std::string getNameRaw() const { return name; };
	const std::string getName()  const { return itemNames[name]; }
	const std::string& getGfxDir()  const noexcept { return gfxDir; }
	const std::string& getSfxDir()  const noexcept { return sfxDir; }
	const int& getNumber() const noexcept { return number; }
	const int& getPrice()  const noexcept { return price;  }
	const int& getRequirements() const noexcept { return this->requirements; }

	//Virtual Functions ----------------------
	virtual const int& getDmgMin()	const noexcept { return zero; }
	virtual const int& getDmgMax()	const noexcept { return zero; }

	virtual const int& getDmgProtection() const noexcept { return zero; }

	virtual const std::string& getEffect() const noexcept { return emptyString; }
	virtual const int& getPower() const noexcept { return zero; }
	virtual void decreaseSpan() noexcept {}
	virtual const int& getMana()  const noexcept { return zero;  }
	virtual const int& getSpan()  const noexcept { return zero; }
};

class Weapon : public Item {
private:
	int dmgMin;
	int dmgMax;

	//Construnctors and a destructor ---------
public:
	Weapon() noexcept {
		type = "weapon";
		name = "WepNONE";
		dmgMin = 0;
		dmgMax = 0;
		requirements = 1;
	}
	~Weapon(){}

	//Functions ------------------------------
	void save(std::ofstream &out) const override {
		json itemInfo{
			{"type", type},
			{"name", name},
			{"gfxDir", gfxDir},
			{"sfxDir", sfxDir},
			{"price", price},
			{"number", number},
			{"requirements", requirements },
			{"dmgMin", dmgMin },
			{"dmgMax", dmgMax }
		};
		out << std::setw(2) << itemInfo;
	}
	void load(std::ifstream &in) override {
		json itemInfo;
		in >> itemInfo;
		this->load(itemInfo);
	}
	void load(const json &itemInfo) override {
		type = itemInfo["type"].get<std::string>();
		name = itemInfo["name"].get<std::string>();
		gfxDir = itemInfo["gfxDir"].get<std::string>();
		sfxDir = itemInfo["sfxDir"].get<std::string>();
		price = itemInfo["price"];
		number = itemInfo["number"];
		requirements = itemInfo["requirements"];
		dmgMin = itemInfo["dmgMin"];
		dmgMax = itemInfo["dmgMax"];
	}
	const std::vector<std::string> loadGfx() const override {
		return files::loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Weapons\\" + gfxDir + ".txt");
	}

	//Accessors ------------------------------
	const int& getDmgMin()	const noexcept override { return this->dmgMin; }
	const int& getDmgMax()	const noexcept override { return this->dmgMax; }
};

class Armor : public Item {
private:
	int dmgProtection;

	//Construnctors and a destructor ---------
public:
	Armor() noexcept {
		type = "armor";
		name = "NONE";
		dmgProtection = 0;
	}
	~Armor(){}

	//Functions ------------------------------
	void save(std::ofstream &out) const override {
		json itemInfo{
			{ "type", type },
			{ "name", name },
			{ "gfxDir", gfxDir },
			{ "sfxDir", sfxDir },
			{ "price", price },
			{ "number", number },
			{ "requirements", requirements },
			{ "dmgProtection", dmgProtection }
		};
		out << std::setw(2) << itemInfo;
	}
	void load(std::ifstream &in) override {
		json itemInfo;
		in >> itemInfo;
		this->load(itemInfo);
	}
	void load(const json &itemInfo) override {
		type = itemInfo["type"].get<std::string>();
		name = itemInfo["name"].get<std::string>();
		gfxDir = itemInfo["gfxDir"].get<std::string>();
		sfxDir = itemInfo["sfxDir"].get<std::string>();
		price = itemInfo["price"];
		number = itemInfo["number"];
		requirements = itemInfo["requirements"];
		dmgProtection = itemInfo["dmgProtection"];
	}

	const std::vector<std::string> loadGfx() const override {
		return files::loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Armors\\" + gfxDir + ".txt");
	}

	//Accessors ------------------------------
	const int& getDmgProtection() const noexcept override { return this->dmgProtection; }
};

class Consumable : public Item {
private:
	std::string effect;
	int power;

	//Construnctors and a destructor ---------
public:
	Consumable() noexcept {
		type = "consumable";
		effect = "NONE";
		power = 0;
		requirements = 1;
	}
	~Consumable(){}

	//Functions ------------------------------
	void save(std::ofstream &out) const override {
		json itemInfo{
			{ "type", type },
			{ "name", name },
			{ "gfxDir", gfxDir },
			{ "sfxDir", sfxDir },
			{ "price", price },
			{ "number", number },
			{ "effect", effect },
			{ "power", power }
		};
		out << std::setw(2) << itemInfo;
	}
	void load(std::ifstream &in) override {
		json itemInfo;
		in >> itemInfo;
		this->load(itemInfo);
	}
	void load(const json &itemInfo) override {
		type = itemInfo["type"].get<std::string>();
		name = itemInfo["name"].get<std::string>();
		gfxDir = itemInfo["gfxDir"].get<std::string>();
		sfxDir = itemInfo["sfxDir"].get<std::string>();
		price  = itemInfo["price"];
		number = itemInfo["number"];
		effect = itemInfo["effect"].get<std::string>();
		power  = itemInfo["power"];
	}
	const std::vector<std::string> loadGfx() const override {
		return files::loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Consumables\\" + gfxDir + ".txt");
	}

	//Accessors ------------------------------
	const std::string& getEffect() const noexcept override { return this->effect; }
	const int& getPower() const noexcept override { return this->power; }
};

class Spell : public Item {
private:
	std::string effect;
	int power;
	int mana;
	int span;

	//Construnctors and a destructor ---------
public:
	Spell() noexcept {
		type = "spell";
		effect = "NONE";
		power = 0;
		requirements = 1;
		span = 0;
		mana = 0;
	}
	~Spell() {}

	//Functions ------------------------------
	void save(std::ofstream &out) const override {
		json itemInfo{
			{ "type", type },
			{ "name", name },
			{ "gfxDir", gfxDir },
			{ "sfxDir", sfxDir },
			{ "price", price },
			{ "number", number },
			{ "requirements", requirements },
			{ "effect", effect },
			{ "power", power },
			{ "mana", mana },
			{ "span", span }
		};
		out << std::setw(2) << itemInfo;		
	}
	void load(std::ifstream &in) override {
		json itemInfo;
		in >> itemInfo;
		this->load(itemInfo);
	}
	void load(const json &itemInfo) override {
		type = itemInfo["type"].get<std::string>();
		name = itemInfo["name"].get<std::string>();
		gfxDir = itemInfo["gfxDir"].get<std::string>();
		sfxDir = itemInfo["sfxDir"].get<std::string>();
		price = itemInfo["price"];
		number = itemInfo["number"];
		requirements = itemInfo["requirements"];
		effect = itemInfo["effect"].get<std::string>();
		power = itemInfo["power"];
		mana = itemInfo["mana"];
		span = itemInfo["span"];
	}
	const std::vector<std::string> loadGfx() const override {
		return files::loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Spells\\" + gfxDir + ".txt");
	}

	//Modifiers ------------------------------
	void decreaseSpan() noexcept override { span--; }

	//Accessors ------------------------------
	const std::string& getEffect() const noexcept override { return this->effect; }
	const int& getPower() const noexcept override { return this->power; }
	const int& getMana() const noexcept override { return this->mana; }
	const int& getSpan() const noexcept override { return this->span; }
};

//Itemp Maps -----------------------------
namespace itms {
	extern Weapon wepNONE;
	extern Armor armorNONE;
	extern std::map<std::string, Weapon> weapons;
	extern std::map<std::string, Armor> armors;
	extern std::map<std::string, Consumable> consumables;
	extern std::map<std::string, Spell> spells;
}

//Finding Items of Specified Type --------
std::vector<std::shared_ptr<Item>> itemTypeFind(const std::vector<std::shared_ptr<Item>>& items, const std::string &type);

//Saving and loading collections ---------
namespace files {
	void save(std::ofstream &out, const std::vector<std::shared_ptr<Item>> &inv);

	template<class T>
	void loadMap(const json &itemTab, std::map<std::string, T> &map) {
		std::shared_ptr<Item> item = std::make_shared<T>();
		map.clear();

		for (const json &e : itemTab["tab"]) {
			item->load(e["itemInfo"]);
			map[e["key"]] = *std::dynamic_pointer_cast<T>(item);
		}
	}
	const std::vector<std::shared_ptr<Item>> load(std::ifstream &in);
	const std::vector<std::shared_ptr<Item>> load(const std::string &fileDir, const std::string &which);
}