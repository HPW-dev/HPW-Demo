#include "scene-record-table.hpp"
#include "scene-manager.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/util/keybits.hpp"
#include "game/util/score-table.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"

Scene_record_table::Scene_record_table()
{ init_menu(); }

void Scene_record_table::init_menu() {
  Menu_items menu_items {
    new_shared<Menu_text_item>(
      //get_locale_str("scene.input.exit_and_save"), TODO locale
      U"save replay",
      [] { hpw_log("need impl. for save replay\n"); } // TODO call save replay
    ),
    new_shared<Menu_text_item>(
      get_locale_str("scene.input.exit_and_save"),
      [] { hpw::scene_mgr->back(); }
    )
  };

  menu = new_shared<Text_menu>(menu_items, Vec{30, 50} );
} // init_menu

void Scene_record_table::update(double dt) {
  menu->update(dt);
}

void Scene_record_table::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);

  // TODO locale
  graphic::font->draw(dst, {30, 30}, U"Рекорд: " + n2s<utf32>(hpw::get_score()), &blend_max);
}
