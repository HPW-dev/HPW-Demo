#include <ranges>
#include <cassert>
#include "scene-nickname.hpp"
#include "game/core/user.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font-util.hpp"

struct Scene_nickname::Impl {
  cautox TEXT_DELETE_TIMEOUT = 120u;
  uint m_text_delete_timer {};

  inline Impl() {
    hpw::text_input = hpw::player_name;
    hpw::text_input_pos = hpw::text_input.size();
    hpw::text_input_mode = true;
  }

  inline ~Impl() {
    hpw::text_input_mode = false;
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit();

    // стирание текста
    if (is_pressed_once(hpw::keycode::text_delete)) {
      if (!hpw::text_input.empty()) {
        hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
          0, hpw::text_input.size());
        hpw::text_input.erase(hpw::text_input_pos, 1);
      }
    }

    // если зажали кнопку удаления текста
    if (is_pressed(hpw::keycode::text_delete)) {
      ++m_text_delete_timer;
      if (m_text_delete_timer >= TEXT_DELETE_TIMEOUT) {
        if (!hpw::text_input.empty() && ((m_text_delete_timer % 6u) == 0u)) {
          hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
            0, hpw::text_input.size());
          hpw::text_input.erase(hpw::text_input_pos, 1);
        }
      }
    } else {
      m_text_delete_timer = 0;
    }

    // перемещение текстового курсора ввода влево
    if (is_pressed_once(hpw::keycode::left)) {
      hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
        0, hpw::text_input.size());
    }

    // перемещение текстового курсора ввода вправо
    if (is_pressed_once(hpw::keycode::right)) {
      hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos + 1,
        0, hpw::text_input.size());
    }

    // подтвердить и выйти
    if (is_pressed_once(hpw::keycode::enable)) {
      hpw::player_name = hpw::text_input;
      exit();
    }
  }

  inline void draw(Image& dst) const {
    assert(graphic::font);
    dst.fill({});

    utf32 txt = get_locale_str("nickname.enter_name") + U": ";
    txt += hpw::text_input;
    if ((graphic::frame_count % 40) > 20)
      txt += U"<";
    txt += U"\n";
    txt += get_locale_str("nickname.press_key") + U" <";
    txt += hpw::keys_info.find(hpw::keycode::enable)->name + U"> ";
    txt += get_locale_str("nickname.for_exit");
    txt += U"\n" + get_locale_str("nickname.copy_paste");
    graphic::font->draw(dst, Vec(25, dst.Y/3 - 6), txt);
  }

  inline static void exit() {
    hpw::text_input_mode = false;
    hpw::scene_mgr.back();
  }
}; // impl

Scene_nickname::Scene_nickname(): impl {new_unique<Impl>()} {}
Scene_nickname::~Scene_nickname() {}
void Scene_nickname::update(const Delta_time dt) { impl->update(dt); }
void Scene_nickname::draw(Image& dst) const { impl->draw(dst); }
