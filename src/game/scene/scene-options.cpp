#include "scene-options.hpp"
#include "scene-mgr.hpp"
#include "scene-input.hpp"
#include "scene-graphic.hpp"
#include "scene-game-options.hpp"
#include "scene-hud-select.hpp"
#include "scene-locale.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/blur-helper.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/image/image.hpp"

Scene_options::Scene_options() {
  init_menu();
}

void Scene_options::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr.back();
  menu->update(dt);

  // чтобы перезагрузить локализацию строк
  if (hpw::scene_mgr.status.came_back)
    init_menu();
}

void Scene_options::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {20, 30}, get_locale_str("options.name"), &blend_max);
}

void Scene_options::init_menu() {
  init_shared<Text_menu>( menu,
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("options.graphic"), []{ hpw::scene_mgr.add(new_shared<Scene_graphic>()); }),
      new_shared<Menu_text_item>(get_locale_str("hud_select.title"), []{ hpw::scene_mgr.add(new_shared<Scene_hud_select>()); }),
      /* TODO new_shared<Menu_text_item>(get_locale_str("options.sound"), []{ hpw_log("need impl. for Sound settings\n"); }), */
      new_shared<Menu_text_item>(get_locale_str("options.input"), []{ hpw::scene_mgr.add(new_shared<Scene_input>()); }),
      new_shared<Menu_text_item>(get_locale_str("game_opts.title"), []{ hpw::scene_mgr.add(new_shared<Scene_game_options>()); }),
      new_shared<Menu_text_item>(hpw::locale_select_title, []{ hpw::scene_mgr.add(new_shared<Scene_locale_select>()); }),
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
    },
    Vec{60, 80}
  );
} // init_menu
