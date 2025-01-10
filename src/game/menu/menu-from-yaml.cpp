#include "menu-from-yaml.hpp"
#include "advanced-text-menu.hpp"
#include "util/file/yaml.hpp"

Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions) {
  assert(config.check());
  assert(!actions.empty());

  Unique<Menu> menu;
  // TODO
  return menu;
}
