#include "scene-difficulty.hpp"
#include "scene-manager.hpp"
#include "scene-loading.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/util/game-replay.hpp"
#include "game/util/replay.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/util/game-util.hpp"

Scene_difficulty::Scene_difficulty() {
  init_menu();
}

void Scene_difficulty::update(double dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  menu->update(dt);
}

void Scene_difficulty::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {30, 40}, U"Заглушка выбора сложности", &blend_max);
}

void Scene_difficulty::init_menu() {
  menu = new_shared<Text_menu>(
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("scene.main_menu.start"), []{
        hpw::scene_mgr->add(new_shared<Scene_loading>( []{
          hpw::replay_read_mode = false;
          hpw::scene_mgr->add(new_shared<Scene_game>());
        } ));
      }),
      new_shared<Menu_bool_item>(get_locale_str("scene.gameover.write_replay"),
        []{ return hpw::enable_replay; },
        [](const bool val) { hpw::enable_replay = val; }
      ),
      // TODO select difficulty item
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr->back();
      }),
    },
    Vec{60, 80}
  );
} // init_menu
