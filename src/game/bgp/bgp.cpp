#include <cassert>
#include <unordered_map>
#include "bgp.hpp"
#include "bgp-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

namespace bgp {

static std::unordered_map<Str, Maker> _table; // здесь конструкторы всех фонов

Shared<Bgp> make(cr<Str> name) {
  try {
    return _table.at(name)();
  } catch (...) {}

  log_error << "не удалось найти фон с именем \"" + name + "\", будет выбран случайный фон";
  return make(random_name());
}

Strs all_names() {
  iferror(_table.empty(), "список фоновых узоров пуст");

  Strs ret;
  for (crauto [name, _]: _table)
    ret.push_back(name);
  return ret;
}

void registrate(cr<Str> name, Maker maker) {
  assert(!name.empty());
  assert(maker);
  _table[name] = maker;
  log_debug << "Добавлен фоновой узор \"" << name << "\"";
}

} // bgp ns
