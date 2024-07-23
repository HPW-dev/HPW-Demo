#include "scene-input.hpp"
#include "scene-mgr.hpp"
#include "host/command.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/core/scenes.hpp"
#include "game/util/game-util.hpp"
#include "game/util/config.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/image/image.hpp"

Scene_input::Scene_input() {
  init_menu();
}

void Scene_input::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  menu->update(dt);
}

void Scene_input::draw(Image& dst) const {
  dst.fill(Pal8::black);
  graphic::font->draw(dst, {30, 30}, get_locale_str("scene.options.input"), &blend_max);
  menu->draw(dst);
}

// хелпер функция для перезаписи клавиши
void add_menu_item(Menu_items& dst, CN<Keys_info> info,
CN<Str> locale_name, hpw::keycode keycode) {
  // найти по коду hpw клавиши инфу о реальной клавиши из GLFW
  auto item = info.find(keycode);
  return_if (!item);
  
  nauto hpw_key = item->hpw_key;
  // создать пункт меню с именем клавиши
  dst.emplace_back( new_shared<Menu_text_item>(
    get_locale_str(locale_name),
    [=] { hpw::rebind_key(hpw_key); },
    [=] { // обновити название клавиши
      auto x = hpw::keys_info.find(hpw_key);
      if (x)
        return x->name;
      return utf32(U"error");
    }
  ));
} // add_menu_item

void Scene_input::init_menu() {
  Menu_items menu_items;

#define AMI(key_name) add_menu_item(menu_items, hpw::keys_info, "scene.input."#key_name, hpw::keycode::key_name)
  AMI(up);
  AMI(down);
  AMI(left);
  AMI(right);
  AMI(shoot);
  AMI(bomb);
  AMI(enable);
  AMI(escape);
  AMI(fulscrn);
  AMI(screenshot);
  AMI(focus);
  AMI(mode);
#undef AMI

  menu_items.emplace_back (new_shared<Menu_text_item> (
    get_locale_str("common.reset"), []{ hpw::reset_keymap(); } ) );

  menu_items.emplace_back (
    new_shared<Menu_text_item>(get_locale_str("scene.input.exit_and_save"), []{
      hpw::scene_mgr->back();
      save_config();
    } )
  );

  init_shared<Text_menu>(menu, menu_items, Vec{30, 45} );
} // init_menu
