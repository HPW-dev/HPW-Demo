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
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "host/command.hpp"

struct Scene_game_options::Impl {
  Unique<Advanced_text_menu> m_menu {};

  inline explicit Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    m_menu->update(dt);
  }

  inline void draw(Image& dst) const {
    m_menu->draw(dst);
  }

  inline void init_menu() {
    Menu_items menu_items {
      //new_shared<Menu_double_item>(
      //  get_locale_str("scene.graphic_menu.gamma.gamma_value"),
      //  []()->double { return graphic::gamma; },
      //  [](const double val) { hpw::set_gamma(val); },
      //  0.005,
      //  get_locale_str("scene.graphic_menu.gamma.description.gamma_value")
      //),
      new_shared<Menu_text_item>( get_locale_str("common.exit"),
        []{ hpw::scene_mgr->back(); } ),
    }; // menu_items

    m_menu = new_unique<Advanced_text_menu>(
      get_locale_str("scene.graphic_menu.gamma.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
  } // init_menu
}; // Impl

Scene_game_options::Scene_game_options(): impl {new_unique<Impl>()} {}
Scene_game_options::~Scene_game_options() {}
void Scene_game_options::update(const Delta_time dt) { impl->update(dt); }
void Scene_game_options::draw(Image& dst) const { impl->draw(dst); }
