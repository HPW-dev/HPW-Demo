#include <iostream>
#include <ctime>
#include "util/str.hpp"
#include "util/rnd-table.hpp"

int main() {
  cauto seed = std::time({});
  set_rnd_seed(seed);

  Rnd_table<Str> table({
    "Test A",
    "Test B",
    "Test C",
    "Test 1",
    "Test 2",
    "Test 3",
  });
  table.push("test 4");

  cnauto value = table.rnd_stable();
  std::cout << "random table value: " << value << std::endl;
}
