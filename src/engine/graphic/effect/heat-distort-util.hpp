#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>

class Heat_distort;

Heat_distort load_heat_distort(const nlohmann::json& config);
