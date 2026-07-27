#include <cassert>
#include <utility>
#include "scene-enter-text.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/sync.hpp"
#include "graphic/image/image.hpp"

struct Scene_enter_text::Impl {
  utf32 _title {};
  utf32 _desc {};
  utf32 _edit_title {};
  Text_processor _proc {};
  cautox TEXT_DELETE_TIMEOUT = 120u;
  uint _text_delete_timer {};

  inline explicit Impl(cr<utf32> title, cr<utf32> desc, cr<utf32> edit_title,
  cr<utf32> stast_text, cr<Text_processor> proc)
  : _title {title}, _desc {desc}, _edit_title {edit_title}, _proc {proc}
  {
    assert(proc);
    hpw::text_input_mode = true;
    hpw::text_input = stast_text;
    hpw::text_input_pos += stast_text.size();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit_from_scene();
    
    if (is_pressed_once(hpw::keycode::enable)) {
      _proc(hpw::text_input);
      hpw::text_input.clear();
      hpw::text_input_mode = false;
      exit_from_scene();
    }

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
      ++_text_delete_timer;
      if (_text_delete_timer >= TEXT_DELETE_TIMEOUT) {
        if (!hpw::text_input.empty() && ((_text_delete_timer % 6u) == 0u)) {
          hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
            0, hpw::text_input.size());
          hpw::text_input.erase(hpw::text_input_pos, 1);
        }
      }
    } else {
      _text_delete_timer = 0;
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
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    assert(graphic::font);
    utf32 text;
    if (!_title.empty())
      text += _title + U"\n";
    if (!_desc.empty())
      text += _desc + U"\n";
    text += _edit_title + hpw::text_input;
    if ((graphic::frame_count % 120) > 60)
      text += U"<";
    graphic::font->draw(dst, {15, 10}, text);
  }

  inline static void exit_from_scene() {
    hpw::scene_mgr.back();
  }
}; // Impl

Scene_enter_text::Scene_enter_text(cr<utf32> title, cr<utf32> desc, cr<utf32> edit_title,
cr<utf32> stast_text, cr<Text_processor> proc)
  : _impl {new_unique<Impl>(title, desc, edit_title, stast_text, proc)} {}
Scene_enter_text::~Scene_enter_text() {}
void Scene_enter_text::update(const Delta_time dt) { _impl->update(dt); }
void Scene_enter_text::draw(Image& dst) const { _impl->draw(dst); }
