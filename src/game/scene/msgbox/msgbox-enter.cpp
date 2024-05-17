#include <cassert>
#include "msgbox-enter.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/canvas.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/util/keybits.hpp"
#include "graphic/font/font.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/blur.hpp"

struct Msgbox_enter::Impl {
  constx int WND_X = 375;
  constx int WND_Y = 110;
  utf32 m_msg {};
  utf32 m_title {};
  Action m_action {};
  Image m_bg {};

  inline Impl(CN<utf32> msg, CN<utf32> title, CN<Action> action)
  : m_msg {msg}
  , m_title {title}
  , m_action {action}
  { make_bg(); }

  inline void update(double dt) {
    if (is_pressed_once(hpw::keycode::enable)
    || is_pressed_once(hpw::keycode::escape)) {
      if (m_action)
        m_action();
      hpw::scene_mgr->back();
    }
  }

  inline void draw(Image& dst) const {
    assert(dst.size == m_bg.size);
    insert_fast(dst, m_bg);
  }

  inline void make_bg() {
    // затенить прошлый кадр игры
    m_bg = *graphic::canvas;
    sub_brightness(m_bg, Pal8::from_real(0.55));

    Rect window(
      (m_bg.X - WND_X) / 2.0,
      (m_bg.Y - WND_Y) / 2.0,
      WND_X, WND_Y
    );
    Rect border(
      window.pos.x + 2,
      window.pos.y + 2,
      window.size.x - 4,
      window.size.y - 4
    );
    
    // заблюрить часть фона под окном
    auto for_blur = fast_cut(m_bg, window.pos.x, window.pos.y, window.size.x, window.size.y);
    adaptive_blur(for_blur, 5);
    insert(m_bg, for_blur, window.pos);

    // нарисовать окно с рамкой
    draw_rect_filled<&blend_alpha>(m_bg, window, Pal8::black, 200);
    draw_rect(m_bg, border, Pal8::gray);

    cauto text_pos = center_point(Vec(m_bg.X, m_bg.Y), graphic::font->text_size(m_msg));
    graphic::font->draw(m_bg, window.pos + Vec(6, 6), m_title, &blend_avr);
    graphic::font->draw(m_bg, text_pos, m_msg);
  } // make_bg
}; // Impl

Msgbox_enter::Msgbox_enter(CN<utf32> msg, CN<utf32> title, CN<Action> action)
  : impl {new_unique<Impl>(msg, title, action)} {}
Msgbox_enter::~Msgbox_enter() {}
void Msgbox_enter::update(double dt) { impl->update(dt); }
void Msgbox_enter::draw(Image& dst) const { impl->draw(dst); }
