#include <cassert>
#include "scene-epge.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"

struct Scene_epge::Impl {
  Unique<Advanced_text_menu> _menu {};

  inline explicit Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit_from_scene();

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    assert(_menu);
    _menu->draw(dst);
  }

  inline static void exit_from_scene() {
    assert(hpw::scene_mgr);
    hpw::scene_mgr->back();
  }

  inline void init_menu() {
    /*
    Menu_items menu_items {
      new_shared<Menu_double_item>(
        get_locale_str("scene.graphic_menu.gamma.gamma_value"),
        []()->double { return graphic::gamma; },
        [](const double val) { hpw::set_gamma(val); },
        0.005,
        get_locale_str("scene.graphic_menu.gamma.description.gamma_value")
      ),
      new_shared<Menu_text_item>( get_locale_str("common.reset"),
        []{ hpw::set_gamma(1.0); } ),
      new_shared<Menu_text_item>( get_locale_str("common.exit"),
        []{ hpw::scene_mgr->back(); } ),
    }; // menu_items

    init_unique( m_menu,
      get_locale_str("scene.graphic_menu.gamma.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
    */
  }
}; // Impl

Scene_epge::Scene_epge(): impl {new_unique<Impl>()} {}
Scene_epge::~Scene_epge() {}
void Scene_epge::update(const Delta_time dt) { impl->update(dt); }
void Scene_epge::draw(Image& dst) const { impl->draw(dst); }
