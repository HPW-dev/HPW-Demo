#include <cassert>
#include "scene-replay-select.hpp"
#include "scene-manager.hpp"
#include "scene-loading.hpp"
#include "game/core/common.hpp"
#include "game/core/scenes.hpp"
#include "game/core/replays.hpp"
#include "game/util/replay.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/table-menu.hpp"
#include "game/menu/item/table-row-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/core/difficulty.hpp"
#include "util/path.hpp"

struct Replay_info {
  Str path {};
  utf32 player_name {};
  Str date {};
  Difficulty difficulty {};
  uint level {};
  int64_t score {};
};

struct Scene_replay_select::Impl {
  Unique<Menu> menu {};
  Vector<Replay_info> m_replay_info_table {};

  inline Impl() {
    load_replays();
    init_menu();
  }

  inline void update(double dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    menu->draw(dst);
  }

  inline void init_menu() {
    iflog (m_replay_info_table.empty(), "вызови перед этим load_replays\n");

    menu = new_unique<Table_menu>(
      get_locale_str("scene.replay.name"),
      Table_menu::Rows {
        Table_menu::Row {.name = get_locale_str("scene.replay.table.player"), .sz = 170},
        Table_menu::Row {.name = get_locale_str("scene.replay.table.date"), .sz = 70},
        Table_menu::Row {.name = get_locale_str("scene.replay.table.difficulty"), .sz = 85},
        Table_menu::Row {.name = get_locale_str("scene.replay.table.levels"), .sz = 60},
        Table_menu::Row {.name = get_locale_str("scene.replay.table.score")},
      },
      25,
      generate_rows()
    );
  } // init_menu

  inline Menu_items generate_rows() const {
    return_if(m_replay_info_table.empty(), {});

    Menu_items ret;
    for (cnauto replay_info: m_replay_info_table) {
      ret.emplace_back( new_shared<Menu_item_table_row>(
        [replay_info] { // запуск файла реплея
          assert(!replay_info.path.empty());
          hpw::scene_mgr->add(new_shared<Scene_loading>( [replay_info]{
            // TODO проследи чтоб реплей грузил настройки сложности и сам их выбирал
            hpw::replay_read_mode = true;
            hpw::cur_replay_file_name = replay_info.path;
            hpw::scene_mgr->add(new_shared<Scene_game>());
          } ));
        },
        Menu_item_table_row::Content_getters {
          [replay_info]->utf32 { return replay_info.player_name; },
          [replay_info]->utf32 { return sconv<utf32>(replay_info.date); },
          [replay_info]->utf32 { return U"TODO"; /*replay_info.difficulty;*/ },
          [replay_info]->utf32 { return n2s<utf32>(replay_info.level); },
          [replay_info]->utf32 { return n2s<utf32>(replay_info.score); },
        }
      ) );
    }
    return ret;
  } // generate_rows

  inline void load_replays() {
    cauto replay_files = files_in_dir(hpw::cur_dir + "replays/");
    return_if(replay_files.empty());

    for (cnauto replay_file: replay_files) {
      m_replay_info_table.emplace_back(Replay_info {
        .path = replay_file,
        .player_name = U"TODO",
        .date = "TODO",
        .difficulty = Difficulty::normal, // TODO
        .level = 0, // TODO
        .score = 0, // TODO
      });
    }
  } // load_replays
}; // impl

Scene_replay_select::Scene_replay_select(): impl {new_unique<Impl>()} {}
Scene_replay_select::~Scene_replay_select() {}
void Scene_replay_select::update(double dt) { impl->update(dt); }
void Scene_replay_select::draw(Image& dst) const { impl->draw(dst); }
