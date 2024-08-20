#include <iostream>
#include "util/file/yaml.hpp"
#include "util/path.hpp"

void write(cr<Str> fname) {
  Yaml root;
  auto test_1 = root["test_1"];
  test_1.set_real("float_val", 9.7997f);
  test_1.set_int("int_val", 97997);
  test_1.set_bool("bool_val", true);
  test_1.set_str("default_test", "defined str");
  test_1.set_v_real("v_real", {0.1, 9.7997, 0, 1000.676});
  auto node_2 = root["node_2"];
  node_2.set_str("asci_str", "HPW-dev");
  node_2.set_utf8("utf8_ru", u8"чё ето?");
  node_2.set_utf8("utf8_ch", u8"是我这来中");
  root.save(fname);
} // write

void read(cr<Str> fname) {
  auto root = Yaml(fname);
  auto node_1 = root["test_1"];
  std::cout << "\nnode 1:\n";
  std::cout << "\tfloat_val:" << node_1.get_real("float_val") << "\n";
  std::cout << "\tint_val:" << node_1.get_int("int_val") << "\n";
  std::cout << "\tbool_val:" << (node_1.get_bool("bool_val") ? "true" : "false") << "\n";
  std::cout << "\tv_real: ";
  for (crauto val: node_1.get_v_real("v_real"))
    std::cout << val << ", ";
  std::cout << "\n";
  auto node_2 = root["node_2"];
  std::cout << "\nnode 2:\n";
  std::cout << "\tasci_str:" << node_2.get_str("asci_str") << "\n";
  std::cout << "\tutf8_ru:" << sconv<std::string>(node_2.get_utf8("utf8_ru")) << "\n";
  std::cout << "\tutf8_ch:" << sconv<std::string>(node_2.get_utf8("utf8_ch")) << "\n";
  std::cout << "\tdefault_str:" << node_2.get_str("default_str", "default text") << "\n";
  std::cout << "\tdefault_int:" << node_2.get_int("default_int", 8800555) << "\n";
} // read

int main(int argc, char *argv[]) {
  auto launch_dir = launch_dir_from_argv0(argv[0]);
  auto fname = launch_dir + "../test/yaml/test.yml";
  write(fname);
  read(fname);
}
