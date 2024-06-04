#include "scene-options.hpp"
#include "scene-manager.hpp"
#include "scene-input.hpp"
#include "scene-graphic.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/image/image.hpp"

Scene_options::Scene_options() {
  init_menu();
}

void Scene_options::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  menu->update(dt);
}

void Scene_options::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {20, 30}, get_locale_str("scene.options.name"), &blend_max);
}

void Scene_options::init_menu() {
  menu = new_shared<Text_menu>(
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("scene.options.graphic"), []{
        hpw::scene_mgr->add(new_shared<Scene_graphic>());
      }),
      new_shared<Menu_text_item>(get_locale_str("scene.options.input"), []{
        hpw::scene_mgr->add(new_shared<Scene_input>());
      }),
      /* TODO
      new_shared<Menu_text_item>(get_locale_str("scene.options.sound"), []{
        hpw_log("need impl. for Sound settings\n");
      }),*/
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr->back();
      }),
    },
    Vec{60, 80}
  );
} // init_menu
