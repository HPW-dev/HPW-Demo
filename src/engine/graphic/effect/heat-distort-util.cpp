#include "pch.hpp"
#include "heat-distort-util.hpp"
#include "heat-distort.hpp"

Heat_distort load_heat_distort(const nlohmann::json& config) {
  Heat_distort ret;
  
  ret.set_duration( config.value("max_duration", real{0}) );
  ret.radius = config.value("radius", int{0});
  ret.block_size = config.value("block_size", int{0});
  ret.block_count = config.value("block_count", int{0});
  ret.power = config.value("power", real{0});

  crauto flags_node = config["flags"];
  ret.flags.random_block_count     = flags_node.value("random_block_count", false);
  ret.flags.random_radius          = flags_node.value("random_radius", false);
  ret.flags.random_block_size      = flags_node.value("random_block_size", false);
  ret.flags.random_power           = flags_node.value("random_power", false);
  ret.flags.infinity_duration      = flags_node.value("infinity_duration", false);
  ret.flags.decrease_radius        = flags_node.value("decrease_radius", false);
  ret.flags.invert_decrease_radius = flags_node.value("invert_decrease_radius", false);
  ret.flags.decrease_power         = flags_node.value("decrease_power", false);
  ret.flags.decrease_block_size    = flags_node.value("decrease_block_size", false);
  ret.flags.repeat                 = flags_node.value("repeat", false);

  return ret;
} // load_heat_distort
