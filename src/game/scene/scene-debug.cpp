#include <ctime>
#include "scene-debug.hpp"
#include "scene-manager.hpp"
#include "game/game-common.hpp"
#include "game/game-core.hpp"
#include "game/game-debug.hpp"
#include "game/game-sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/int-item.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "util/file/archive.hpp"
#include "util/math/random.hpp"
#include "host/host-util.hpp"

Scene_debug::Scene_debug() {
  init_menu();
}

void Scene_debug::update(double dt) {
  // выйти из дебага по ESC или по той же кнопке
  if (is_pressed_once(hpw::keycode::escape) || is_pressed_once(hpw::keycode::debug))
    hpw::scene_mgr->back();

  menu->update(dt);
}

void Scene_debug::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
}

void Scene_debug::init_menu() {
  menu = new_shared<Text_menu>(
    Menu_items {
      new_shared<Menu_bool_item>(U"I/U/FPS",
        [] { return graphic::show_debug_info; },
        [] (bool new_val) { graphic::show_debug_info = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Entity mem",
        [] { return hpw::show_entity_mem_map; },
        [] (bool new_val) { hpw::show_entity_mem_map = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Step mode",
        [] { return graphic::step_mode; },
        [] (bool new_val) {
          graphic::step_mode = new_val;
          hpw::scene_mgr->back();
        }
      ),
      new_shared<Menu_bool_item>(U"Hitboxes",
        [] { return graphic::draw_hitboxes; },
        [] (bool new_val) { graphic::draw_hitboxes = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Collider grids",
        [] { return graphic::show_grids; },
        [] (bool new_val) { graphic::show_grids = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Inputs",
        [] { return graphic::draw_controls; },
        [] (bool new_val) { graphic::draw_controls = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Entity pos",
        [] { return graphic::draw_entity_pos; },
        [] (bool new_val) { graphic::draw_entity_pos = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Draw entities",
        [] { return graphic::draw_entitys; },
        [] (bool new_val) { graphic::draw_entitys = new_val; }
      ),
      new_shared<Menu_bool_item>(U"Draw level bg",
        [] { return graphic::draw_level; },
        [] (bool new_val) { graphic::draw_level = new_val; }
      ),
      new_shared<Menu_bool_item>(U"+ update lag",
        [] { return hpw::update_delay; },
        [] (bool new_val) { hpw::update_delay = new_val; }
      ),
      new_shared<Menu_bool_item>(U"+ render lag",
        [] { return hpw::render_delay; },
        [] (bool new_val) { hpw::render_delay = new_val; }
      ),
      new_shared<Menu_int_item>(
        U"Target UPS",
        [] { return hpw::target_ups; },
        [] (int new_val) { set_target_ups(std::clamp(new_val, 10, 1'000)); },
        10
      ),
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr->back();
      }),
    },
    Vec{20, 30} // menu draw pos
  ); // make menu
} // init_menu
