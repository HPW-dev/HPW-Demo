#include <cassert>
#include <unordered_map>
#include "menu-from-yaml.hpp"
#include "advanced-text-menu.hpp"
#include "menu/item/text-item.hpp"
#include "util/file/yaml.hpp"
#include "util/unicode.hpp"
#include "util/error.hpp"
#include "game/util/locale.hpp"
#include "game/core/scenes.hpp"

inline static Shared<Menu_item> make_text_item(cr<Yaml> item_node, cr<Action_table> actions) {
  // добавочное имя к адресу локализации
  cauto locale_prefix = item_node.get_str("locale_prefix");

  // если title/desc не задан, брать по умолчанию из префикса
  cauto title = get_locale_str(item_node.get_str("title", locale_prefix + ".title"));
  cauto desc = get_locale_str(item_node.get_str("desc", locale_prefix + ".desc"));

  // колбэк при выборе пункта
  Menu_text_item::Action action;
  cauto action_name = item_node.get_str("action");
  iferror (action_name.empty(), "требуется действие на кнопке");
  try {
    action = actions.at(action_name).cast_to<decltype(action)>();
  } catch (...) {
    error("ошибка при получении \"" << action_name << "\"");
  }

  // колбэк для получения значения
  Menu_text_item::Getter getter;
  cauto getter_name = item_node.get_str("getter");
  if (!getter_name.empty()) {
    try {
      getter = actions.at(getter_name).cast_to<decltype(getter)>();
    } catch (...) {
      error("ошибка при получении \"" << getter_name << "\"");
    }
  }

  return new_shared<Menu_text_item>(title, action, getter, desc);
}

inline static Shared<Menu_item> make_back_item(cr<Yaml> item_node, cr<Action_table> actions)
  { return new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }); }
  
inline static Shared<Menu_item> make_exit_item(cr<Yaml> item_node, cr<Action_table> actions)
  { return new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }); }

inline static Shared<Menu_item> make_menu_item(cr<Yaml> item_node, cr<Action_table> actions) {
  assert(item_node.check());
  cauto type = item_node.get_str("type", "text_item");
  ret_if(type == "text_item", make_text_item(item_node, actions));
  ret_if(type == "back_item", make_back_item(item_node, actions));
  ret_if(type == "exit_item", make_exit_item(item_node, actions));
  error("неизвестный тип элемента меню: \"" << type << "\"");
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
    auto item = make_menu_item(item_node, actions);
    assert(item);
    items.emplace_back(std::move(item));
  }

  return new_unique<Advanced_text_menu>(title, items, rect, atm_config);
}
