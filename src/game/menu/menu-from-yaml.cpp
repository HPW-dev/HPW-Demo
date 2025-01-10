#include <cassert>
#include "menu-from-yaml.hpp"
#include "advanced-text-menu.hpp"
#include "util/file/yaml.hpp"
#include "util/unicode.hpp"

inline static Shared<Menu_item> make_menu_item(cr<Yaml> item_node) {
  assert(item_node.check());
  return {};
}

Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions) {
  assert(config.check());
  assert(!actions.empty());

  utf32 title = utf8_to_32(config.get_str("title"));
  Rect rect {};
  Advanced_text_menu_config atm_config {};

  Menu_items items {};
  cauto items_node = config["items"];
  assert(items_node.check());
  for (crauto item_node: items_node.items()) {
    auto item = make_menu_item(item_node);
    assert(item);
    items.emplace_back(std::move(item));
  }

  return new_unique<Advanced_text_menu>(title, items, rect, atm_config);
}
