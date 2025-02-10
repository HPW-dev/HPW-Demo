#include <cassert>
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
   neu::Simple_config::Input_neuron {.name = "test", .getter = []->neu::Weight { return 0; }},
  };
  config.hiden_layers = {
   neu::Simple_config::Hiden_layer{.hiden_neurons = 2},
   neu::Simple_config::Hiden_layer{.hiden_neurons = 2},
  };
  config.outputs.neurons = {
   neu::Simple_config::Output_neuron {.name = "test", .setter = [](neu::Weight){}},
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

Unique<neu::Simple> make_xor_net(neu::Weight& a, neu::Weight& b, neu::Weight& dst) {
  neu::Simple_config config;
  config.inputs.neurons = {
    neu::Simple_config::Input_neuron {.name = "a", .getter = [&]->neu::Weight { return a; }},
    neu::Simple_config::Input_neuron {.name = "b", .getter = [&]->neu::Weight { return b; }},
  };
  config.hiden_layers = {
    neu::Simple_config::Hiden_layer{.hiden_neurons = 3},
    neu::Simple_config::Hiden_layer{.hiden_neurons = 2},
  };
  config.outputs.neurons = {
    neu::Simple_config::Output_neuron {.name = "result", .setter = [&](const neu::Weight out){ dst = out; }},
  };

  auto net = new_unique<neu::Simple>(config);
  neu::randomize(*net);
  return net;
}

void xor_test() {
  hpw_info("-- NEU: XOR TEST --\n");

  struct Item { neu::Weight a {}, b {}, res {}; };
  Vector<Item> table {
    Item {.a=0, .b=0, .res=0},
    Item {.a=1, .b=0, .res=1},
    Item {.a=0, .b=1, .res=1},
    Item {.a=1, .b=1, .res=0},
  };
  neu::Weight a;
  neu::Weight b;
  neu::Weight result;

  auto net = make_xor_net(a, b, result);
  assert(net);

  for (crauto item: table) {
    a = item.a;
    b = item.b;
    net->update();
    hpw_info("a = " + n2s(a, 0) + ", b = " + n2s(b, 0) + ", xor result = " + n2s(result, 5) + "\n");
  }
}

int main() {
  softmax_test();
  randomization_test();
  xor_test();
}