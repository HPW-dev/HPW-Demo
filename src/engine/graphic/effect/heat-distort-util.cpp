#include "pch.hpp"
#include "heat-distort-util.hpp"
#include "heat-distort.hpp"

Heat_distort load_heat_distort(const nlohmann::json& config) {
  Heat_distort ret;
  
  ret.set_duration( config["max_duration"].get<real>() );
  ret.radius = config["radius"].get<int>();
  ret.block_size = config["block_size"].get<int>();
  ret.block_count = config["block_count"].get<int>();
  ret.power = config["power"].get<real>();

  auto flags_node = config["flags"];
  ret.flags.random_block_count     = flags_node["random_block_count"].get<bool>();
  ret.flags.random_radius          = flags_node["random_radius"].get<bool>();
  ret.flags.random_block_size      = flags_node["random_block_size"].get<bool>();
  ret.flags.random_power           = flags_node["random_power"].get<bool>();
  ret.flags.infinity_duration      = flags_node["infinity_duration"].get<bool>();
  ret.flags.decrease_radius        = flags_node["decrease_radius"].get<bool>();
  ret.flags.invert_decrease_radius = flags_node["invert_decrease_radius"].get<bool>();
  ret.flags.decrease_power         = flags_node["decrease_power"].get<bool>();
  ret.flags.decrease_block_size    = flags_node["decrease_block_size"].get<bool>();
  ret.flags.repeat                 = flags_node["repeat"].get<bool>();

  return ret;
} // load_heat_distort
