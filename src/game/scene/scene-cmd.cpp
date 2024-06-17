#include <cassert>
#include <utility>
#include <functional>
#include "scene-cmd.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/user.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/messages.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/cmd.hpp"
#include "game/util/game-util.hpp"

struct Scene_cmd::Impl {
  // сколько апдейтов надо зажимать удаление текста, чтобы он начал удаляться
  constx auto TEXT_DELETE_TIMEOUT = 120u;
  Image bg {};
  uint m_text_delete_timer {};

  inline ~Impl() { hpw::text_input_mode = false; }

  inline Impl() {
    hpw::text_input_mode = true;
    hpw::text_input.clear();
    // фон - предыдущий кадр
    bg = *graphic::canvas;
  }

  inline void update(const Delta_time dt) {
    // выход
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    if (is_pressed_once(hpw::keycode::console))
      hpw::scene_mgr->back();
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
    // выполнить команду
    if (is_pressed_once(hpw::keycode::enable)) {
      hpw::cmd->exec( sconv<Str>(hpw::text_input) );
      hpw::text_input.clear();
    }
    // загрузить предыдущую команду
    if (is_pressed_once(hpw::keycode::up)) {
      hpw::text_input = sconv<utf32>(hpw::cmd->last_command());
      hpw::text_input_pos = hpw::text_input.size();
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
    // дописать команду из предлагаемых при автодописывании
    if (is_pressed_once(hpw::keycode::focus)) {
      cauto input = sconv<Str>(hpw::text_input);
      cauto matches = hpw::cmd->command_matches(input);
      if (!matches.empty()) {
        hpw::text_input = sconv<utf32>( matches.at(0) );
        hpw::text_input_pos = hpw::text_input.size();
      }
    }
  } // update

  inline void draw(Image& dst) const {
    insert_fast(dst, bg);
    sub_brightness(dst, 30);
    hpw::message_mgr->draw(dst);
    sub_brightness(dst, 110);
    print_input(dst);
    print_autocompletion(dst);
  }

  inline void print_input(Image& dst) const {
    const Vec pos(15, 15);
    utf32 text = hpw::text_input;
    // мигающий курсор
    if ((graphic::frame_count % 30) >= 15) {
      hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos, 0, text.size());
      text.insert(hpw::text_input_pos, 1, U'<');
    }
    graphic::font->draw(dst, pos, U"Команда: " + text);
  }

  // показывает автодополнение команд
  inline void print_autocompletion(Image& dst) const {
    const Vec pos (15, 30);
    cauto input = sconv<Str>(hpw::text_input);
    cauto matches = hpw::cmd->command_matches(input);
    utf32 text = U"________________________________\n";
    for (cnauto match: matches)
      text += sconv<utf32>(match) + U'\n';
    graphic::font->draw(dst, pos, text);
  }
}; // impl

Scene_cmd::Scene_cmd(): impl {new_unique<Impl>()} {}
Scene_cmd::~Scene_cmd() {}
void Scene_cmd::update(const Delta_time dt) { impl->update(dt); }
void Scene_cmd::draw(Image& dst) const { impl->draw(dst); }
