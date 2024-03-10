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
#include "util/path.hpp"

struct Scene_replay_select::Impl {
  Unique<Menu> menu {};
  Strs m_replay_names {};
  uint m_replay_name_idx {};

  inline Impl() {
    init_menu();
    load_replay_names();
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
    /*menu = new_unique<Text_menu>(
      Menu_items {

        new_shared<Menu_text_item>( get_locale_str("scene.replay.play"), [this]{
          return_if (m_replay_names.empty());

          hpw::scene_mgr->add(new_shared<Scene_loading>( [this]{
            // TODO проследи чтоб реплей грузил настройки сложности и сам их выбирал
            hpw::replay_read_mode = true;
            hpw::cur_replay_file_name = get_replay_name();
            hpw::scene_mgr->add(new_shared<Scene_game>());
          } ));
        } ),

        // TODO сделай удобнее
        new_shared<Menu_text_item>(U"следующий файл", [this]{ next_replay_file(); } ),

        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
          hpw::scene_mgr->back(); // to main menu
        }),

      }, // Menu_items

      Vec{60, 80}
    );*/

    menu = new_unique<Table_menu>(
      get_locale_str("scene.replay.title"),
      Table_menu::Rows {
        Table_menu::Row {.name = U"player", .sz = 100},
        Table_menu::Row {.name = U"date",   .sz = 50},
        Table_menu::Row {.name = U"mode",   .sz = 100},
        Table_menu::Row {.name = U"levels", .sz = 100},
        Table_menu::Row {.name = U"score"},
      },
      25,
      generate_rows()
    );
  } // init_menu

  inline Menu_items generate_rows() const {
    Menu_items ret;

    // TODO через std::bind передай какие файлы запускаются в action

    // TODO del:
    auto item = new_shared<Menu_item_table_row>(
      []{ hpw_log("it work! (TODO)\n"); },
      Menu_item_table_row::Content_getters {
        []->utf32 { return U"test 1"; },
        []->utf32 { return U"test 2"; },
        []->utf32 { return U"test 3"; }
      }
    );
    ret.push_back(item);
    ret.push_back(item);
    ret.push_back(item);

    return ret;
  } // generate_rows

  inline void next_replay_file() {
    return_if(m_replay_names.empty());
    m_replay_name_idx = (m_replay_name_idx + 1) % m_replay_names.size();
  }

  inline Str get_replay_name() const {
    if (m_replay_names.empty())
      return "-";

    auto it = m_replay_names.cbegin();
    std::advance(it, m_replay_name_idx);
    if (it == m_replay_names.cend())
      return "-";

    return *it;
  }

  inline void load_replay_names()
    { m_replay_names = files_in_dir(hpw::cur_dir + "replays/"); }

}; // impl

Scene_replay_select::Scene_replay_select(): impl {new_unique<Impl>()} {}
Scene_replay_select::~Scene_replay_select() {}
void Scene_replay_select::update(double dt) { impl->update(dt); }
void Scene_replay_select::draw(Image& dst) const { impl->draw(dst); }
