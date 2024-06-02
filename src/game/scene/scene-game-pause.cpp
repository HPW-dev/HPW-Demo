#include "scene-game-pause.hpp"
#include "scene-manager.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/replay.hpp"
#include "game/core/replays.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-options.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "host/command.hpp"

Scene_game_pause::Scene_game_pause() {
  init_menu();
}

void Scene_game_pause::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back(); // возврат в игру
  menu->update(dt);
}

void Scene_game_pause::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {30, 40}, U"Заглушка паузы игры", &blend_max);
}

void Scene_game_pause::init_menu() {
  menu = new_shared<Text_menu>(
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("common.continue"), []{
        hpw::scene_mgr->back();
      }),
      new_shared<Menu_text_item>(get_locale_str("scene.options.name"), []{
        hpw::scene_mgr->add(new_shared<Scene_options>());
      }),
      new_shared<Menu_text_item>(get_locale_str("scene.pause.screenshot"), []{
        hpw::make_screenshot();
      }),
      new_shared<Menu_text_item>(get_locale_str("scene.pause.main_menu"), []{
        hpw::scene_mgr->back(); // выйти из паузы
        hpw::scene_mgr->back(); // выйти из игрового процесса
        hpw::scene_mgr->back(); // выйти из окна загрузки
        // в реплее в выбор сложности не заходим (TODO может поменяться)
        if (!hpw::replay_read_mode)
          hpw::scene_mgr->back(); // выйти из меню выбора сложности
      }),
      new_shared<Menu_text_item>(get_locale_str("scene.pause.game_exit"), []{
        hpw::soft_exit();
      }),
    },
    Vec{60, 80}
  );
} // init_menu
