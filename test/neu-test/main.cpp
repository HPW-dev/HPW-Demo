#include <iomanip>
#include <sstream>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/neu/neu-simple.hpp"
#include "util/neu/neu-util.hpp"

void softmax_test() {
  hpw_info("-- NEU: SOFTMAX TEST --\n");

  auto ret = neu::softmax( neu::Weights{-1, 2, 4, -2} );
  std::stringstream txt;
  txt << "softmax for [-1, 2, 4, -2] = [";

  for (crauto x: ret)
    txt << n2s(x, 2) << ", ";

  txt << "]\n";
  hpw_info(txt.str());
}

void randomization_test() {
  hpw_info("-- NEU: RANDOMIZATION TEST --\n");

  neu::Simple_config config;
  config.inputs.neurons = {
    neu::Simple_config::Input_neuron {.name = "test", .setter = [](neu::Weight){}},
  };
  config.hiden_layers = {
    neu::Simple_config::Hiden_layer{.hiden_neurons = 2},
    neu::Simple_config::Hiden_layer{.hiden_neurons = 2},
  };
  config.outputs.neurons = {
    neu::Simple_config::Output_neuron {.name = "test", .getter = []->neu::Weight { return 0; }},
  };

  neu::Simple net(config);
  neu::randomize(net);
  std::stringstream txt;
  txt << "randomized weights: [";

  for (crauto w: net.weights())
    txt << n2s(w, 2) << ", ";

  txt << "]\n";
  hpw_info(txt.str());
}

void xor_test() {
  hpw_info("-- NEU: XOR TEST --\n");

  // ...
}

int main() {
  softmax_test();
  randomization_test();
  xor_test();
}