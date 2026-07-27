#include "scene-record-table.hpp"
#include "scene-mgr.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/score-table.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "util/log.hpp"

Scene_record_table::Scene_record_table()
{ init_menu(); }

void Scene_record_table::init_menu() {
  Menu_items menu_items {
    new_shared<Menu_text_item>(
      //get_locale_str("input.exit_and_save"), TODO locale
      U"save replay",
      [] { log_error << "need impl. for save replay"; } // TODO call save replay
    ),
    new_shared<Menu_text_item>(
      get_locale_str("input.exit_and_save"),
      [] { hpw::scene_mgr.back(); }
    )
  };

  init_shared<Text_menu>(menu, menu_items, Vec{30, 50} );
} // init_menu

void Scene_record_table::update(const Delta_time dt) {
  menu->update(dt);
}

void Scene_record_table::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);

  // TODO locale
  graphic::font->draw(dst, {30, 30}, U"Рекорд: " + n2s<utf32>(hpw::get_score_normalized()), &blend_max);
}
