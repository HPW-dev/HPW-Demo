#include "scene-difficulty.hpp"
#include "game/game-difficulty.hpp"
#include "scene-manager.hpp"
#include "scene-loading.hpp"
#include "graphic/image/image.hpp"
#include "game/game-canvas.hpp"
#include "game/game-common.hpp"
#include "game/util/game-replay.hpp"
#include "game/util/replay.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/scene/scene-game.hpp"
#include "util/str-util.hpp"

Scene_difficulty::Scene_difficulty() {
  hpw::difficulty = Difficulty::normal;
  init_menu();
}

void Scene_difficulty::update(double dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  menu->update(dt);
}

void Scene_difficulty::draw(Image& dst) const {
  menu->draw(dst);
}

void Scene_difficulty::init_menu() {
  menu = new_shared<Advanced_text_menu>(
    get_locale_str("scene.difficulty_select.title"),
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("scene.difficulty_select.start"), []{
        hpw::scene_mgr->add(new_shared<Scene_loading>( []{
          hpw::replay_read_mode = false;
          hpw::scene_mgr->add(new_shared<Scene_game>());
          hpw_log("выбран уровень сложности: " << n2s( scast<int>(hpw::difficulty) ) << "\n");
        } ));
      }),
      new_shared<Menu_list_item>(get_locale_str("scene.difficulty_select.difficulty.title"),
        Menu_list_item::Items {
          Menu_list_item::Item {
            .name = get_locale_str("scene.difficulty_select.difficulty.normal"),
            .desc = get_locale_str("scene.difficulty_select.description.difficulty.normal"),
            .action = []{ hpw::difficulty = Difficulty::normal; }
          },
          Menu_list_item::Item {
            .name = get_locale_str("scene.difficulty_select.difficulty.easy"),
            .desc = get_locale_str("scene.difficulty_select.description.difficulty.easy"),
            .action = []{ hpw::difficulty = Difficulty::easy; }
          },
          Menu_list_item::Item {
            .name = get_locale_str("scene.difficulty_select.difficulty.hardcore"),
            .desc = get_locale_str("scene.difficulty_select.description.difficulty.hardcore"),
            .action = []{ hpw::difficulty = Difficulty::hardcore; }
          }
        } // Items
      ), // List items
      new_shared<Menu_bool_item>(get_locale_str("scene.difficulty_select.write_replay"),
        []{ return hpw::enable_replay; },
        [](const bool val) { hpw::enable_replay = val; },
        get_locale_str("scene.difficulty_select.description.write_replay")
      ),
      // TODO select difficulty item
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr->back();
      }),
    },
    Rect(Vec{}, Vec(graphic::width, graphic::height))
  );
} // init_menu
