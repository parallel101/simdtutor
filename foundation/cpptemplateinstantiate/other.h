#pragma once

#include <string>
#include <variant>

using namespace std;

using Object = variant<int, double, string>;

void add_object(Object o);
void print_objects();
