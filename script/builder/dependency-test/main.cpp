#include "funcs/func_1.hpp"
#include "funcs/func_2.hpp"

#include <iostream>

int main() {
  std::cout << "func1 + func2 = " << func_1(77) + func_2(33.44) << std::endl;
}
