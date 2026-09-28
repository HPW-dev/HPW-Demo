#include "pch.hpp"
#include "json-util.hpp"

nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name) {
   dst[tag_name] = nlohmann::json::object();
   return dst[tag_name];
}
