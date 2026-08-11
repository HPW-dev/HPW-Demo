#include <cstdio>
#include "module-1.hpp"
#include "module-2.hpp"
#include "module-3.hpp"
#include "module-4.hpp"

int main() {
  unsigned long ret = 97997;
  ret += func_1(ret);
  ret += func_2(ret);
  ret += func_3(ret);
  ret += func_4(ret);
  printf("result: %lu", ret);
}
