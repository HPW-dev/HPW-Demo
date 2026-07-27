#include <cassert>
#include "menu-from-yaml.hpp"
#include "menu.hpp"
#include "advanced-text-menu.hpp"
#include "menu/item/text-item.hpp"
#include "util/file/yaml.hpp"
#include "util/unicode.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "game/util/locale.hpp"
#include "game/core/scenes.hpp"
#include "game/scene/scene-locale.hpp"

inline static Shared<Menu_item> make_text_item(cr<Yaml> item_node, cr<Action_table> actions) {
  // добавочное имя к адресу локализации
  cauto locale_prefix = item_node.get_str("locale_prefix");

  // если title/desc не задан, брать по умолчанию из префикса
  cauto title = get_locale_str(item_node.get_str("title", locale_prefix + ".title"));
  utf32 desc {};
  if (!locale_prefix.empty()) {
    cauto tmp_desc = get_locale_str_with_check(item_node.get_str("desc", locale_prefix + ".desc"));
    if (tmp_desc)
      desc = tmp_desc.value();
  }

  // колбэк при выборе пункта
  Action action;
  cauto action_name = item_node.get_str("action");

  try {
    action = actions.at(action_name);
  } catch (...) {
    log_error << "не удалось привязать действие \"" << action_name << "\" для кнопки \"" << utf32_to_8(title) << "\"";
  }

  #ifndef RELEASE
  cauto getter_name = item_node.get_str("getter");
  iferror(!getter_name.empty(), "этот функционал был удалён 04.05.2025");
  #endif

  auto getter = []->utf32 { return {}; };
  return new_shared<Menu_text_item>(title, action, getter, desc);
}

inline static Shared<Menu_item> make_back_item(cr<Yaml> item_node, cr<Action_table> actions)
  { return new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }); }

inline static Shared<Menu_item> make_locale_select_item(cr<Yaml> item_node, cr<Action_table> actions)
  { return new_shared<Menu_text_item>(hpw::locale_select_title, []{ hpw::scene_mgr.add(new_shared<Scene_locale_select>()); }); }

inline static Shared<Menu_item> make_exit_item(cr<Yaml> item_node, cr<Action_table> actions)
  { return new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }); }

inline static Shared<Menu_item> make_menu_item(cr<Yaml> item_node, cr<Action_table> actions) {
  assert(item_node.check());
  cauto type = item_node.get_str("type", "text_item");
  ret_if(type == "text_item", make_text_item(item_node, actions));
  ret_if(type == "locale_select", make_locale_select_item(item_node, actions));
  ret_if(type == "back_item", make_back_item(item_node, actions));
  ret_if(type == "exit_item", make_exit_item(item_node, actions));
  error("неизвестный тип элемента меню: \"" << type << "\"");
  return {};
}

Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions) {
  assert(config.check());

  utf32 title = get_locale_str(config.get_str("title"));
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
