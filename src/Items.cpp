#include "Items.hpp"

using namespace std;

//Itemp Maps -----------------------------
namespace itms {
	map<string, Weapon> weapons;
	map<string, Armor> armors;
	map<string, Consumable> consumables;
	map<string, Spell> spells;
}

//Finding Items of Specified Type --------
vector<shared_ptr<Item>> itemTypeFind(const vector<shared_ptr<Item>>& items, const string &type) {
	vector<shared_ptr<Item>> typeItems;

	if (items.size() == 0) return typeItems;

	for (const shared_ptr<Item> &e : items) {
		if (e->getType() == type) typeItems.push_back(e);
	}
	return typeItems;
}

//Saving and loading collections ---------m
namespace files {
	void save(ofstream &out, const vector <shared_ptr<Item>> &inv) {
		const json size{
			{"inventorySize", inv.size()}
		};
		out << "\n\n" << size;

		for (const shared_ptr<Item> &e : inv) {
			out << '\n';
			e->save(out);
		}
	}

	const vector<shared_ptr<Item>> load(ifstream &in) {
		json itemInfo;
		vector <shared_ptr<Item>> inv;
		Weapon wepTemp;
		Armor armorTemp;
		Consumable consumTemp;
		Spell spellTemp;

		json size;
		in >> size;
		for (int i = 0; i < size["inventorySize"]; i++) {
			in >> itemInfo;
			if (itemInfo["type"] == "weapon") {
				wepTemp.load(itemInfo);
				inv.push_back(make_shared<Weapon>(wepTemp));
			}
			else if (itemInfo["type"] == "armor") {
				armorTemp.load(itemInfo);
				inv.push_back(make_shared<Armor>(armorTemp));
			}
			else if (itemInfo["type"] == "consumable") {
				consumTemp.load(itemInfo);
				inv.push_back(make_shared<Consumable>(consumTemp));
			}
			else if (itemInfo["type"] == "spell") {
				spellTemp.load(itemInfo);
				inv.push_back(make_shared<Spell>(spellTemp));
			}

			itemInfo.clear();
		}

		return inv;
	}
	const vector<shared_ptr<Item>> load(const string &filedir, const string &which) {
		json fileInv;
		vector <shared_ptr<Item>> inv;
		Weapon wepTemp;
		Armor armorTemp;
		Consumable consumTemp;
		Spell spellTemp;
		string category;

		ifstream in(filedir);
		in >> fileInv;
		in.close();

		for (const json &j : fileInv["merchants"]) {
			if (j["key"] == which) {
				for (const string& s : j["inventory"]) {
					if (s == "consumables" || s == "weapons" || s == "armors" || s == "spells") {
						category = s;
					}
					else {
						if (category == "consumables")  { inv.push_back(make_shared<Consumable>(itms::consumables[s])); }
						else if (category == "weapons") { inv.push_back(make_shared<Weapon>(itms::weapons[s])); }
						else if (category == "armors")  { inv.push_back(make_shared<Armor>(itms::armors[s])); }
						else if (category == "spells")  { inv.push_back(make_shared<Spell>(itms::spells[s])); }
					}
				}
				break;
			}
		}

		return inv;
	}
}