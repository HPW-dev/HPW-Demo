#include <cassert>
#include <unordered_map>
#include "bgp.hpp"
#include "bgp-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

namespace bgp {

using Table = std::unordered_map<Str, Maker>;

static Table& _get_table() {
  static Table _table; // здесь конструкторы всех фонов
  return _table;
}

Shared<Bgp> make(cr<Str> name) {
  try {
    return _get_table().at(name)();
  } catch (...) {}

  log_error << "не удалось найти фон с именем \"" + name + "\", будет выбран случайный фон";
  return make(random_name());
}

Strs all_names() {
  iferror(_get_table().empty(), "список фоновых узоров пуст");

  Strs ret;
  for (crauto [name, _]: _get_table())
    ret.push_back(name);
  return ret;
}

void registrate(cr<Str> name, Maker maker) {
  assert(!name.empty());
  assert(maker);
  _get_table()[name] = maker;
  log_debug << "Добавлен фоновой узор \"" << name << "\"";
}

} // bgp ns
