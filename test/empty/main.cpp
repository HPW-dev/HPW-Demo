#include <iostream>
#include "util/str.hpp"
#include "util/push-pop.hpp"

int main() {
  Str src = "before";
  std::cout << "src = " << src << std::endl;
  {
    push_pop _(src);
    src = "after";
    std::cout << "src = " << src << std::endl;
  }
  std::cout << "src = " << src << std::endl;
}
