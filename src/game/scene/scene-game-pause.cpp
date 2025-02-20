#include "scene-game-pause.hpp"
#include "scene-mgr.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sounds.hpp"
#include "game/util/keybits.hpp"
#include "game/util/palette-helper.hpp"
#include "game/util/locale.hpp"
#include "game/util/palette-helper.hpp"
#include "game/core/replays.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/scene/scene-options.hpp"
#include "graphic/image/image.hpp"
#include "host/command.hpp"

Scene_game_pause::Scene_game_pause() {
  init_menu();
  hpw::sound_mgr->stop_all();
}

void Scene_game_pause::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape)) {
    hpw::scene_mgr.back(); // возврат в игру
    hpw::sound_mgr->continue_all();
  }
  menu->update(dt);
}

void Scene_game_pause::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {30, 40}, U"Заглушка паузы игры", &blend_max);
}

void Scene_game_pause::init_menu() {
  init_shared<Text_menu>( menu,
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("common.continue"),
        []{ hpw::scene_mgr.back(); }),
      new_shared<Menu_text_item>(get_locale_str("options.title"),
        []{ hpw::scene_mgr.add(new_shared<Scene_options>()); }),
      new_shared<Menu_text_item>(get_locale_str("pause.screenshot"),
        []{ hpw::make_screenshot(); }),
      get_palette_list(),
      new_shared<Menu_text_item>(get_locale_str("pause.main_menu"), []{
        hpw::scene_mgr.back(3); // cur->пауза->игровой процесс->окно загрузки
        // в реплее в выбор сложности не заходим (TODO может поменяться)
        if (!hpw::replay_read_mode) {
          hpw::scene_mgr.back(); // выйти из меню выбора сложности
          hpw::replay = {};
        }
      }),
      new_shared<Menu_text_item>(get_locale_str("pause.game_exit"),
        []{ hpw::soft_exit(); }),
    },
    Vec{60, 80}
  );
} // init_menu
