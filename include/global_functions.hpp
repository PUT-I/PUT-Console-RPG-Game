#pragma once

#include "item.hpp"

std::vector<std::shared_ptr<item>> item_type_find(const std::vector<std::shared_ptr<item>>& items,
                                                  const item::item_type_enum& type);
int rand_int(const int& min, const int& max);
