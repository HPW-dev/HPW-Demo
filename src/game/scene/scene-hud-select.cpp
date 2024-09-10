#include <cassert>
#include "scene-hud-select.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/graphic.hpp"
#include "game/core/canvas.hpp"
#include "game/core/huds.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/hud/hud-util.hpp"
#include "util/error.hpp"

struct Scene_hud_select::Impl {
  Unique<Advanced_text_menu> _menu {}; // менюшка выбора

  inline explicit Impl() {
    const Rect MENU_WND(Vec(30, 30), Vec(210, 160));
    cauto TITLE = get_locale_str("scene.options.hud.title");

    Menu_items items;
    // имена HUD'ов как кнопки
    cauto huds = hud_names();
    for (crauto hud_name: huds) {
      items.push_back(
        new_shared<Menu_bool_item>(
          utf8_to_32(hud_name),
          [name = hud_name]{ return graphic::cur_hud == name; },
          [name = hud_name](bool _){
            graphic::cur_hud = name;
            graphic::hud = make_hud(name);
          }
        )
      );
    }
    // дефол гуй
    items.push_back(new_shared<Menu_text_item>(get_locale_str("scene.options.hud.default"),
      []{ graphic::hud = make_hud("asci"); }));
    // выход из меню
    items.push_back(new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr->back(); }));

    init_unique(_menu, TITLE, items, MENU_WND, true);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::red);
    _menu->draw(dst);
  }
}; // Impl

Scene_hud_select::Scene_hud_select(): impl {new_unique<Impl>()} {}
Scene_hud_select::~Scene_hud_select() {}
void Scene_hud_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_hud_select::draw(Image& dst) const { impl->draw(dst); }
