#include "pch.hpp"
#include "json-util.hpp"

nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name) {
   dst[tag_name] = nlohmann::json::object();
   return dst[tag_name];
}

Strs root_tags(cr<nlohmann::json> src) {
  Strs ret;
  for (crauto [tag, data]: src)
    ret.push_back(tag);
  return ret;
}
