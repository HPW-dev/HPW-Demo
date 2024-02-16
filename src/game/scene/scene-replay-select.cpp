#include "scene-replay-select.hpp"
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
#include "game/util/game-util.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
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
    dst.fill(Pal8::black);
    menu->draw(dst);
    // TODO locale
    graphic::font->draw(dst, {30, 40}, U"Выбор реплея", &blend_max);
    draw_selected_replay(dst);
  }

  inline void init_menu() {
    menu = new_unique<Text_menu>(
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
    );
  } // init_menu

  inline void draw_selected_replay(Image& dst) const {
    utf32 txt = U"выбран файл реплея: " + sconv<utf32>(get_replay_name());
    graphic::font->draw(dst, Vec(15, 150), txt);
  }

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

    return get_filename(*it);
  }

  inline void load_replay_names()
    { m_replay_names = files_in_dir(hpw::cur_dir + "replays/"); }

}; // impl

Scene_replay_select::Scene_replay_select(): impl {new_unique<Impl>()} {}
Scene_replay_select::~Scene_replay_select() {}
void Scene_replay_select::update(double dt) { impl->update(dt); }
void Scene_replay_select::draw(Image& dst) const { impl->draw(dst); }
