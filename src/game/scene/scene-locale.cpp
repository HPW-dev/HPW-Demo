#include <cassert>
#include "scene-locale.hpp"
#include "game/core/scenes.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/menu/table-menu.hpp"
#include "game/menu/item/table-row-item.hpp"
#include "util/file/archive.hpp"

struct Scene_locale_select::Impl {
  Unique<Table_menu> menu {};

  inline Impl() {
    cauto title = get_locale_str("scene.locale_select.title");
    cauto row_height = 25u;
    cauto elems_empty_txt = get_locale_str("common.empty");
    Table_menu::Rows rows {
      {get_locale_str("scene.locale_select.translate"), 330},
      {get_locale_str("common.author"), 0},
    };
    Menu_items items {};
    init_unique(menu, title, rows, row_height, items, elems_empty_txt);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();

    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    //dst.fill(Pal8::black);
    menu->draw(dst);
  }
}; // impl

Scene_locale_select::Scene_locale_select(): impl {new_unique<Impl>()} {}
Scene_locale_select::~Scene_locale_select() {}
void Scene_locale_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_locale_select::draw(Image& dst) const { impl->draw(dst); }

