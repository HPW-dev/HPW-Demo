#include <cassert>
#include "scene-game-options.hpp"
#include "scene-nickname.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "util/error.hpp"
#include "host/command.hpp"

struct Scene_game_options::Impl {
  Unique<Advanced_text_menu> m_menu {};

  inline explicit Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      goto_back();

    m_menu->update(dt);
  }

  inline void draw(Image& dst) const { m_menu->draw(dst); }

  inline void init_menu() {
    Menu_items menu_items {
      new_shared<Menu_text_item>(
        get_locale_str("nickname.title"),
        []{ hpw::scene_mgr.add(new_shared<Scene_nickname>()); }
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("game_opts.rnd_pal.title"),
        []{ return hpw::rnd_pal_after_death; },
        [](bool val) { hpw::rnd_pal_after_death = val; },
        get_locale_str("game_opts.rnd_pal.desc")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("game_opts.collider_autoopt.title"),
        []{ return hpw::collider_autoopt; },
        [](bool val) { hpw::collider_autoopt = val; },
        get_locale_str("game_opts.collider_autoopt.desc")
      ),
      priority_menu_item(),
      new_shared<Menu_text_item>(
        get_locale_str("common.exit"),
        [this]{ goto_back(); }
      ),
    }; // menu_items

    init_unique( m_menu,
      get_locale_str("game_opts.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
  } // init_menu

  // выходит из этого меню
  inline void goto_back() {
    hpw::scene_mgr.back();
  }

  // пункт меню с настройкой приоритета процесса
  static inline Shared<Menu_item> priority_menu_item() {
    return new_shared<Menu_list_item>(
      get_locale_str("game_opts.priority.title"),
      Menu_list_item::Items {
        Menu_list_item::Item {
          get_locale_str("game_opts.priority.low"),
          get_locale_str("game_opts.priority.low_desc"),
          []{ set_priority( (hpw::process_priority = Priority::low) ); }
        },
        Menu_list_item::Item {
          get_locale_str("game_opts.priority.normal"),
          get_locale_str("game_opts.priority.normal_desc"),
          []{ set_priority( (hpw::process_priority = Priority::normal) ); }
        },
        Menu_list_item::Item {
          get_locale_str("game_opts.priority.high"),
          get_locale_str("game_opts.priority.high_desc"),
          []{ set_priority( (hpw::process_priority = Priority::high) ); }
        },
        Menu_list_item::Item {
          get_locale_str("game_opts.priority.realtime"),
          get_locale_str("game_opts.priority.realtime_desc"),
          []{ set_priority( (hpw::process_priority = Priority::realtime) ); }
        },
      },
      []{ return scast<std::size_t>(hpw::process_priority); }
    );
  }
}; // Impl

Scene_game_options::Scene_game_options(): impl {new_unique<Impl>()} {}
Scene_game_options::~Scene_game_options() {}
void Scene_game_options::update(const Delta_time dt) { impl->update(dt); }
void Scene_game_options::draw(Image& dst) const { impl->draw(dst); }
