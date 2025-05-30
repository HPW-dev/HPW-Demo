#include <cassert>
#include <algorithm>
#include "scene-replay-select.hpp"
#include "scene-mgr.hpp"
#include "scene-loading.hpp"
#include "game/core/common.hpp"
#include "game/core/scenes.hpp"
#include "game/core/replays.hpp"
#include "game/util/replay.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/difficulty-helper.hpp"
#include "game/menu/table-menu.hpp"
#include "game/menu/item/table-row-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/level/level-tutorial.hpp"
#include "util/log.hpp"
#include "util/path.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"

struct Scene_replay_select::Impl {
  Unique<Menu> menu {};
  Vector<Replay::Info> m_replay_info_table {};

  inline Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    menu->draw(dst);
  }

  inline void init_menu() {
    load_replays();

    init_unique<Table_menu>( menu,
      get_locale_str("replay.name"),
      Table_menu::Rows {
        Table_menu::Row {.name = get_locale_str("replay.table.player"), .sz = 200},
        Table_menu::Row {.name = get_locale_str("replay.table.date"), .sz = 135},
        Table_menu::Row {.name = get_locale_str("replay.table.difficulty"), .sz = 80},
        Table_menu::Row {.name = get_locale_str("replay.table.score")},
      },
      25,
      generate_rows(),
      get_locale_str("replay.no_replay")
    );
  } // init_menu

  inline Menu_items generate_rows() {
    return_if(m_replay_info_table.empty(), {});
    // сортировать реплеи по дате создания
    std::sort(m_replay_info_table.begin(), m_replay_info_table.end(),
      &Impl::date_comparator);

    Menu_items ret;
    for (crauto replay_info: m_replay_info_table) {
      ret.emplace_back( new_shared<Menu_item_table_row>(
        [replay_info] { // запуск файла реплея
          iferror(replay_info.path.empty(), "отсутствует строка пути к файлу реплея");
          hpw::scene_mgr.add(new_shared<Scene_loading>( [replay_info]{
            hpw::replay_read_mode = true;
            hpw::cur_replay_file_name = replay_info.path;
            hpw::scene_mgr.add (
              new_shared<Scene_game>(replay_info.first_level_is_tutorial)
            );
          } ));
        },
        Menu_item_table_row::Content_getters {
          [replay_info]->utf32 { return replay_info.player_name; },
          [replay_info]->utf32 { return sconv<utf32>(replay_info.date_str); },
          [replay_info]->utf32 { return difficulty_to_str(replay_info.difficulty); },
          [replay_info]->utf32 { return n2s<utf32>(replay_info.score); },
        }
      ) );
    }
    return ret;
  } // generate_rows

  inline void load_replays() {
    cauto replay_files = files_in_dir(hpw::cur_dir + hpw::replays_path);
    return_if(replay_files.empty());
    for (crauto replay_file: replay_files) {
      try {
        cauto info = Replay::get_info(replay_file);
        m_replay_info_table.push_back(info);
      } catch (cr<hpw::Error> error) {
        log_error << "не удалось загрузить один из реплеев: " << error.what();
      } catch (...) {
        log_error << "не удалось загрузить один из реплеев";
      }
    }
  } // load_replays

  // для сравнения времени создания реплея
  inline static bool date_comparator(cr<Replay::Info> a, cr<Replay::Info> b) {
    // год
    if (a.date.year < b.date.year) return false;
    if (a.date.year > b.date.year) return true;
    // месяц
    if (a.date.month < b.date.month) return false;
    if (a.date.month > b.date.month) return true;
    // день
    if (a.date.day < b.date.day) return false;
    if (a.date.day > b.date.day) return true;
    // час
    if (a.date.hour < b.date.hour) return false;
    if (a.date.hour > b.date.hour) return true;
    // минута
    if (a.date.minute < b.date.minute) return false;
    if (a.date.minute > b.date.minute) return true;
    // секунда
    return a.date.second > b.date.second;
  } // date_comparator
}; // impl

Scene_replay_select::Scene_replay_select(): impl {new_unique<Impl>()} {}
Scene_replay_select::~Scene_replay_select() {}
void Scene_replay_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_replay_select::draw(Image& dst) const { impl->draw(dst); }
