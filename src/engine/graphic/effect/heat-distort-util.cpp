#include "heat-distort-util.hpp"
#include "heat-distort.hpp"
#include "util/file/yaml.hpp"

Heat_distort load_heat_distort(const Yaml& config) {
  Heat_distort ret;
  
  ret.set_duration( config.get_real("max_duration") );
  ret.radius = config.get_int("radius");
  ret.block_size = config.get_int("block_size");
  ret.block_count = config.get_int("block_count");
  ret.power = config.get_real("power");

  auto flags_node = config["flags"];
  ret.flags.random_block_count     = flags_node.get_bool("random_block_count");
  ret.flags.random_radius          = flags_node.get_bool("random_radius");
  ret.flags.random_block_size      = flags_node.get_bool("random_block_size");
  ret.flags.random_power           = flags_node.get_bool("random_power");
  ret.flags.infinity_duration      = flags_node.get_bool("infinity_duration");
  ret.flags.decrease_radius        = flags_node.get_bool("decrease_radius");
  ret.flags.invert_decrease_radius = flags_node.get_bool("invert_decrease_radius");
  ret.flags.decrease_power         = flags_node.get_bool("decrease_power");
  ret.flags.decrease_block_size    = flags_node.get_bool("decrease_block_size");
  ret.flags.repeat                 = flags_node.get_bool("repeat");

  return ret;
} // load_heat_distort
