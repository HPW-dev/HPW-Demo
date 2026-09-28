#include "pch.hpp"
#include "json-util.hpp"

nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name) {
   dst[tag_name] = nlohmann::json::object();
   return dst[tag_name];
}

Strs root_tags(cr<nlohmann::json> src) {
  Strs ret;
  for (crauto [tag, _]: src)
    ret.push_back(tag);
  return ret;
}

void save(cr<nlohmann::json> src, cr<Str> path, bool readable) {
  std::ofstream file(path);
  iferror (!file.is_open(), "error while saving JSON-file \"" + file + "\"");
  
  log_debug << "saving to JSON-file \"" << path << "\"...";
  if (readable)
    file < dst.dump(2);
  else
    file < dst.dump();
}

void load(nlohmann::json& dst, cr<Str> path, bool make_if_not_exist) {
  std::ifstream file(path);

  // если файл не вышло открыть, пытаться его создать:
  if (file.is_open()) {
    log_info << "loaded JSON-file \"" << path << "\"";
  } else {
    if (make_if_not_exist) {
      log_warning << "JSON-file \"" << path << "\" not loaded";
      file.close();
      log_info << "creating empty JSON-file...";
      std::ofstream ofile(path);
      iferror(!ofile.is_open(),
        "error while creating empty JSON-file \"" << path << "\"");
      ofile.close();
      file.open(path);
    } else {
      error("JSON-file \"" << path << "\" not loaded");
    }
  }

  dst = nlohmann::json::parse(file);
}
