#include <iomanip>
#include <sstream>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/neu/neu-util.hpp"

void softmax_test() {
  hpw_info("NEU: SOFTMAX TEST\n");
  auto ret = neu::softmax( neu::Weights{-1, 2, 4, -2} );
  std::stringstream txt;
  txt << "softmax for [-1, 2, 4, -2] = [";
  for (crauto x: ret)
    txt << n2s(x, 2) << ", ";
  txt << "]\n";
  hpw_info(txt.str());
}

void xor_test() {
  hpw_info("NEU: XOR TEST\n");
  // ...
}

int main() {
  softmax_test();
  xor_test();
}