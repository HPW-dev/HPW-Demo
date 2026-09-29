#include "pch.hpp"
#include "json-util.hpp"

nlohmann::json& make_node(nlohmann::json& dst, cr<Str> tag_name) {
  /* emplace возвращает <итератор, bool>.
  Итератор указывает на элемент созданный или уже существовавший */
  auto [it, inserted] = dst.emplace(tag_name, nlohmann::json::object());
  return it.value();
}

Strs root_tags(cr<nlohmann::json> src) {
  Strs ret;
  for (crauto [tag, _]: src.items())
    ret.push_back(tag);
  return ret;
}

void save(cr<nlohmann::json> src, cr<Str> path, bool readable) {
  std::ofstream file(path);
  iferror (!file.is_open(), "error while saving JSON-file \"" + path + "\"");
  
  log_info << "saving to JSON-file \"" << path << "\"...";
  if (readable)
    file << src.dump(2);
  else
    file << src.dump();
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
      ofile << "{}\n";
      ofile.close();
      file.open(path);
    } else {
      error("JSON-file \"" << path << "\" not loaded");
    }
  }

  try {
    dst = nlohmann::json::parse(file);
  } catch (cr<nlohmann::json::parse_error> e) {
    const Str msg = "error while parsing JSON data from \"" + path + "\":\n"
      + "  " + e.what();
    error(msg);
  }
}

// рекурсивный обход для get_string_table_utf32
inline static void _get_string_table_utf32(
String_table_utf32& table, cr<Str> key, cr<nlohmann::json> value) {
  ret_if (key == "info"); // иногрим версию

  if (value.is_string()) {
    table.emplace_back(Kv_utf32{
      .key = str_tolower(key),
      .str = utf8_to_32(value.get<Str>())
    });
  } elif (value.is_object()) {
    for (crauto [k, v]: value.items())
      _get_string_table_utf32(table, key + "." + k, v);
  } else {
    error("WTF?");
  }
}

String_table_utf32 get_string_table_utf32(cr<nlohmann::json> src) {
  String_table_utf32 ret;
  for (crauto [key, value]: src.items())
    _get_string_table_utf32(ret, key, value);
  return ret;
}

bool check(cr<nlohmann::json> node) { return !node.empty(); }

nlohmann::json node_or_empty(cr<nlohmann::json> node, cr<Str> tag) {
  if (node.contains(tag))
    return node[tag];
  
  return nlohmann::json::object();
}
