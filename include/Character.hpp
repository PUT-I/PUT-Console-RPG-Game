#pragma once

#include "FrequentHeaders.h"
#include "Interface.hpp"

class Character {
protected:
	std::string type{};

	//Location -------------------------------
	int location;
	int subLocation;

	//Levelling ------------------------------
	int level;
	int exp;
	int expNext;

	std::string name{};

	//Stats1 ---------------------------------
	int hp;			//Health
	int sp;			//Stamina
	int mp;			//Mana
	int hpMax;
	int spMax;
	int mpMax;

	//Dependent stats ------------------------
	int baseDmgMin;
	int baseDmgMax;
	int dmgMin;
	int dmgMax;
	int baseDefence;
	int defence;
	int evasionChance;
	int hitChance;

	//Stats2 ---------------------------------
	int strength;	//Increases base dmg, allows using better weapons
	int endurance;	//Increases base defence
	int willpower;	//Allows using better spells and decreases spell cast cost
	int agility;	//Increases evasion and hit chance, decreases stamina cost

	int stat1Points;
	int stat2Points;

	//Equipment -----------------------------
	std::vector<std::shared_ptr<Item>> inventory;
	Weapon weapon;
	Armor  armor;

	std::vector<Spell> activeSpells{};

	int money = 0;

	//Construnctor and a destructor ----------
public:
	Character() noexcept;
	virtual ~Character();

	//Functions ------------------------------
	void clear();
	void refreshItemDependent() noexcept;
	virtual void refreshStatDependent() noexcept;

	void stats1Setup();
	void stats2Setup();
	void initialize(const std::string &name_);

	void display() const;
	void displayDependent() const;

	void addExp(const int &exp_);
	void levelUp();

	//Modifiers ------------------------------
	void setLocation(const int& var) noexcept;
	void setSubLocation(const int& var) noexcept;
	void addToInventory(const std::shared_ptr<Item> &item);
	void addToInventory(const Weapon &item);
	void addToInventory(const Armor &item);
	void addToInventory(const Consumable &item);
	void addToInventory(const Consumable &item, const unsigned int &num);
	void addToInventory(const Spell &item);
	void removeFromInventory();

	void equip(const std::shared_ptr<Item> &item);

private:
	void equip(const Weapon &item);
	void equip(const Armor &item);

public:
	void consume(const std::shared_ptr<Item> &item);
	void consume(const Consumable &item);

	void sortInventory();

	void addToActiveSpells(const Spell& spell);
	void activeSpellsRefresh();
	void castSpell(const Spell &spell, const std::shared_ptr<Character> &enemy);
private:
	void castSpell(const Spell &spell);
	void getSpell(const Spell &spell);
public:
	void clearActiveSpells() noexcept;

	/*------------------------------ Accessors ------------------------------*/
	const std::string& getType() const noexcept;

	//Location Accessors ---------------------
	const int& getLocation() const noexcept;
	const int& getSubLocation() const noexcept;

	const std::string& getName() const noexcept;

	//Levelling Accessors---------------------
	const int& getLevel() const noexcept;
	const int& getExp() const noexcept;
	const int& getExpNext() const noexcept;

	//Stats1 Accessors -----------------------
	const int& getHp() const noexcept;
	const int& getHpMax() const noexcept;
	const int& getSp() const noexcept;
	const int& getSpMax() const noexcept;
	const int& getMp() const noexcept;
	const int& getMpMax() const noexcept;

	//Dependent stats Accessors --------------
	const int& getBaseDmgMin() const noexcept;
	const int& getBaseDmgMax() const noexcept;
	const int& getDmgMin() const noexcept;
	const int& getDmgMax() const noexcept;
	const int& getDefence() const noexcept;
	const int& getHitC() const noexcept;
	const int& getEvasionC() const noexcept;

	//Stats2 Accessors -----------------------
	const int& getStrength() const noexcept;
	const int& getEndurance() const noexcept;
	const int& getWillpower() const noexcept;
	const int& getAgility() const noexcept;
	const int& getStat1Points() const noexcept;
	const int& getStat2Points() const noexcept;

	//Equipment Accessors --------------------
	const std::vector<std::shared_ptr<Item>>& getInventory() const noexcept;
	std::vector<std::shared_ptr<Item>>& getInventory() noexcept;
	const Weapon& getWeapon() const noexcept;
	const Armor& getArmor() const noexcept;
	Armor& getArmor() noexcept;
	const int& getMoney() const noexcept;
	/*-----------------------------------------------------------------------*/


	//Saving|Loading -------------------------
	void save() const;
	void load(const std::string &name_);

	//Other Modifiers ------------------------
	void addHp(const int &inc) noexcept;
	void addSp(const int &inc) noexcept;
	void addMp(const int &inc) noexcept;
	void getDamaged(const int &dmg) noexcept;
	void decreaseSp(const int &dec) noexcept;
	void decreaseMoney(const int &dec) noexcept;
	void increaseDef(const int &inc) noexcept;
	void increaseEv(const int &inc) noexcept;
	void addMoney(const int &inc) noexcept;
	void replaceInventory(const std::vector<std::shared_ptr<Item>>& items);

	const std::vector<Spell>& getActiveSpells() const noexcept;
};

class Enemy : public Character {
private:
	//Additional Dependent stats -------------
	int baseEvasionChance;
	int baseHitChance;
	std::string gfxDir{};
	std::string sfxDir{};


	//Construnctors and a destructor ---------
public:
	Enemy() noexcept;
	~Enemy();

	//Functions ------------------------------
	void refreshStatDependent() noexcept override;
	const std::vector<std::string> loadGfx() const;
	void load(const json &enemyInfo);

	//Accessors ------------------------------
	const std::string& getGfxDir() const noexcept;
	const std::string& getSfxDir() const noexcept;
	const std::string loadName() const;
};

//----------------------- Enemies -----------------------//
extern std::map<std::string, Enemy> enemies;

namespace files {
	void loadGraphics();
}