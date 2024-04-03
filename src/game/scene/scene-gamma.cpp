#include <cassert>
#include "scene-gamma.hpp"
#include "scene-manager.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/double-item.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "host/command.hpp"

struct Scene_gamma::Impl {
  Unique<Advanced_text_menu> m_menu {};
  Sprite* test_image {};

  inline explicit Impl() {
    init_menu();
    test_image = hpw::store_sprite->find("resource/image/other/gamma test.png").get();
    assert(test_image);
  } // impl

  inline void update(double dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    m_menu->update(dt);
  }

  inline void draw(Image& dst) const {
    m_menu->draw(dst);
    insert<&blend_diff>(dst, *test_image, {graphic::width - test_image->X() - 15, 60});
  }

  inline void init_menu() {
    Menu_items menu_items {
      new_shared<Menu_double_item>(
        get_locale_str("scene.graphic_menu.gamma.gamma_value"),
        []()->double { return graphic::gamma; },
        [](const double val) { hpw::set_gamma(val); },
        0.001,
        get_locale_str("scene.graphic_menu.gamma.description.gamma_value")
      ),
      new_shared<Menu_text_item>( get_locale_str("common.exit"),
        []{ hpw::scene_mgr->back(); } ),
    }; // menu_items

    m_menu = new_unique<Advanced_text_menu>(
      get_locale_str("scene.graphic_menu.gamma.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
  } // init_menu
}; // Impl

Scene_gamma::Scene_gamma(): impl {new_unique<Impl>()} {}
Scene_gamma::~Scene_gamma() {}
void Scene_gamma::update(double dt) { impl->update(dt); }
void Scene_gamma::draw(Image& dst) const { impl->draw(dst); }
