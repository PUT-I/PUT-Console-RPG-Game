#include "global_functions.hpp"
#include <random>

using namespace std;

// Finding Items of Specified Type --------
vector<shared_ptr<item>> item_type_find(const vector<shared_ptr<item>>& items, const item::item_type_enum& type)
{
	vector<shared_ptr<item>> type_items;

	if (items.size() == 0) return type_items;

	for (const shared_ptr<item>& e : items)
	{
		if (e->get_type() == type) type_items.push_back(e);
	}
	return type_items;
}

int rand_int(const int& min, const int& max)
{
	if (max <= min) { return min; }
	random_device rd;
	mt19937 gen(rd());
	const uniform_int_distribution<int> d(min, max);
	return d(gen);
}