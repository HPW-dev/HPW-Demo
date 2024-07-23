#include <cassert>
#include "scene-game-options.hpp"
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
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
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

      new_shared<Menu_bool_item>(
        get_locale_str("scene.game_opts.rnd_pal.title"),
        []{ return hpw::rnd_pal_after_death; },
        [](bool val) { hpw::rnd_pal_after_death = val; },
        get_locale_str("scene.game_opts.rnd_pal.desc")
      ),

      new_shared<Menu_text_item>(
        get_locale_str("common.exit"),
        [this]{ goto_back(); }
      ),

    }; // menu_items

    init_unique( m_menu,
      get_locale_str("scene.game_opts.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
  } // init_menu

  // выходит из этого меню
  inline void goto_back() {
    hpw::scene_mgr->back();
  }
}; // Impl

Scene_game_options::Scene_game_options(): impl {new_unique<Impl>()} {}
Scene_game_options::~Scene_game_options() {}
void Scene_game_options::update(const Delta_time dt) { impl->update(dt); }
void Scene_game_options::draw(Image& dst) const { impl->draw(dst); }
