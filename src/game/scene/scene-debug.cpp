#include <ctime>
#include "scene-debug.hpp"
#include "scene-mgr.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"
#include "game/core/debug.hpp"
#include "game/core/scenes.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/int-item.hpp"
#include "game/entity/player/player.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "util/file/archive.hpp"
#include "util/math/random.hpp"
#include "host/host-util.hpp"

Scene_debug::Scene_debug() {
  init_menu();
}

void Scene_debug::update(const Delta_time dt) {
  // выйти из дебага по ESC или по той же кнопке
  if (is_pressed_once(hpw::keycode::escape) || is_pressed_once(hpw::keycode::debug))
    hpw::scene_mgr.back();

  menu->update(dt);
}

void Scene_debug::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
}

void Scene_debug::init_menu() {
  init_shared<Text_menu>( menu,
    Menu_items {
      new_shared<Menu_bool_item>(U"отладочные тайминги",
        [] { return graphic::show_fps; },
        [] (bool new_val) { graphic::show_fps = new_val; }
      ),
      new_shared<Menu_bool_item>(U"отладочные графики",
        [] { return graphic::draw_plots; },
        [] (bool new_val) { graphic::draw_plots = new_val; }
      ),
      new_shared<Menu_bool_item>(U"распределение памяти объектов",
        [] { return hpw::show_entity_mem_map; },
        [] (bool new_val) { hpw::show_entity_mem_map = new_val; }
      ),
      new_shared<Menu_bool_item>(U"игнорить противников",
        [] {
          cauto player = hpw::entity_mgr->get_player();
          if (player)
            return player->status.ignore_enemy;
          return false;
        },
        [] (bool new_val) {
          cauto player = hpw::entity_mgr->get_player();
          if (player) {
            player->status.ignore_enemy = new_val;
            player->status.ignore_bullet = new_val;
          }
        }
      ),
      new_shared<Menu_bool_item>(U"пошаговый режим",
        [] { return graphic::step_mode; },
        [] (bool new_val) {
          graphic::step_mode = new_val;
          hpw::scene_mgr.back();
        }
      ),
      new_shared<Menu_bool_item>(U"хитбоксы объектов",
        [] { return graphic::draw_hitboxes; },
        [] (bool new_val) { graphic::draw_hitboxes = new_val; }
      ),
      new_shared<Menu_bool_item>(U"сетка системы коллизий",
        [] { return graphic::show_grids; },
        [] (bool new_val) { graphic::show_grids = new_val; }
      ),
      new_shared<Menu_bool_item>(U"кнопки ввода",
        [] { return graphic::show_virtual_joystick; },
        [] (bool new_val) { graphic::show_virtual_joystick = new_val; }
      ),
      new_shared<Menu_bool_item>(U"жизни объектов",
        [] { return graphic::draw_entity_hp; },
        [] (bool new_val) { graphic::draw_entity_hp = new_val; }
      ),
      new_shared<Menu_bool_item>(U"координаты объектов",
        [] { return graphic::draw_entity_pos; },
        [] (bool new_val) { graphic::draw_entity_pos = new_val; }
      ),
      new_shared<Menu_bool_item>(U"рисовать объекты",
        [] { return graphic::draw_entities; },
        [] (bool new_val) { graphic::draw_entities = new_val; }
      ),
      new_shared<Menu_bool_item>(U"рисовать фон уровня",
        [] { return graphic::draw_level; },
        [] (bool new_val) { graphic::draw_level = new_val; }
      ),
      new_shared<Menu_bool_item>(U"искусственные лаги тика",
        [] { return hpw::update_delay; },
        [] (bool new_val) { hpw::update_delay = new_val; }
      ),
      new_shared<Menu_bool_item>(U"искусственные лаги рендера",
        [] { return hpw::render_delay; },
        [] (bool new_val) { hpw::render_delay = new_val; }
      ),
      new_shared<Menu_int_item>(
        U"тикрейт",
        [] { return hpw::target_ups; },
        [] (int new_val) { set_target_ups(std::clamp(new_val, 10, 1'000)); },
        10
      ),
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr.back();
      }),
    },
    Vec{20, 30} // menu draw pos
  ); // make menu
} // init_menu
