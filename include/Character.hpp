#pragma once

#include "FrequentHeaders.h"
#include "Interface.hpp"

class Character{
protected:
	std::string type;

	//Location -------------------------------
	int location;
	int subLocation;

	//Levelling ------------------------------
	int level;
	int exp;
	int expNext;

	std::string name;

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

	std::vector<Spell> activeSpells;

	int money = 0;

	//Construnctor and a destructor ----------
public:
	Character() noexcept;
	virtual ~Character() {}

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
	void setLocation(const int &var)	noexcept { this->location = var; }
	void setSubLocation(const int &var) noexcept { this->subLocation = var; }
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

	void addToActiveSpells(Spell spell);
	void activeSpellsRefresh();
	void castSpell(const Spell &spell, const std::shared_ptr<Character> &enemy);
private:
	void castSpell(const Spell &spell);
	void getSpell(const Spell &spell);
public:
	void clearActiveSpells() noexcept;

	/*------------------------------ Accessors ------------------------------*/
	const std::string& getType() const noexcept { return type; }

	//Location Accessors ---------------------
	const int& getLocation()	 const noexcept { return this->location;	}
	const int& getSubLocation()	 const noexcept { return this->subLocation;	}

	const std::string& getName() const noexcept { return this->name; }

	//Levelling Accessors---------------------
	const int& getLevel()	const noexcept { return this->level;	}
	const int& getExp()		const noexcept { return this->exp;		}
	const int& getExpNext() const noexcept { return this->expNext;	}

	//Stats1 Accessors -----------------------
	const int& getHp()		const noexcept { return this->hp;	 }
	const int& getHpMax()	const noexcept { return this->hpMax; }
	const int& getSp()		const noexcept { return this->sp;	 }
	const int& getSpMax()	const noexcept { return this->spMax; }
	const int& getMp()		const noexcept { return this->mp;	 }
	const int& getMpMax()	const noexcept { return this->mpMax; }

	//Dependent stats Accessors --------------
	const int& getBaseDmgMin()	const noexcept { return this->baseDmgMin;	}
	const int& getBaseDmgMax()	const noexcept { return this->baseDmgMax;	}
	const int& getDmgMin()		const noexcept { return this->dmgMin;		}
	const int& getDmgMax()		const noexcept { return this->dmgMax;		}
	const int& getDefence()		const noexcept { return this->defence;		}
	const int& getHitC()		const noexcept { return this->hitChance;	}
	const int& getEvasionC()	const noexcept { return this->evasionChance;}
	
	//Stats2 Accessors -----------------------
	const int& getStrength()	const noexcept { return this->strength;		}
	const int& getEndurance()	const noexcept { return this->endurance;	}
	const int& getWillpower()	const noexcept { return this->willpower;	}
	const int& getAgility()		const noexcept { return this->agility;		}
	const int& getStat1Points() const noexcept { return this->stat1Points;	}
	const int& getStat2Points() const noexcept { return this->stat2Points;	}

	//Equipment Accessors --------------------
	const std::vector<std::shared_ptr<Item>>& getInventory() const noexcept { return this->inventory; }
	std::vector<std::shared_ptr<Item>>& getInventory() noexcept { return this->inventory; }
	const Weapon& getWeapon() const noexcept { return this->weapon; }
	const Armor&  getArmor()  const noexcept { return this->armor; }
	Armor&  getArmor()  noexcept { return this->armor; }
	const int& getMoney() const noexcept { return money; }
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
	void replaceInventory(const std::vector<std::shared_ptr<Item>> &items) { inventory = items; }

	const std::vector<Spell>& getActiveSpells() noexcept { return activeSpells; }
};

class Enemy : public Character {
private:
	//Additional Dependent stats -------------
	int baseEvasionChance;
	int baseHitChance;
	std::string gfxDir;
	std::string sfxDir;


	//Construnctors and a destructor ---------
public:
	Enemy() noexcept {
		baseEvasionChance = 0;
		baseHitChance = 0;
		name = "NONE";
		type = "enemy";
		gfxDir = "NONE";
	}
	~Enemy(){}

	//Functions ------------------------------
	void refreshStatDependent() noexcept override;
	const std::vector<std::string> loadGfx() const {
		return files::loadGfx("GameFiles\\Scenarios\\" + currentScenario + "\\Resources\\Graphics\\Creatures\\" + gfxDir + ".txt");
	}
	void load(const json &enemyInfo);

	//Accessors ------------------------------
	const std::string& getGfxDir() const noexcept { return gfxDir; }
	const std::string& getSfxDir() const noexcept { return sfxDir; }
	const std::string  loadName()  const { return enemyNames[name]; }
};

//----------------------- Enemies -----------------------//
extern std::map<std::string, Enemy> enemies;

namespace files {
	void loadGraphics();
}