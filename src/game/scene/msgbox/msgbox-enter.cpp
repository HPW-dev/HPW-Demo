#include <cassert>
#include "msgbox-enter.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/replays.hpp"
#include "game/core/graphic.hpp"
#include "game/core/canvas.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/font/font-util.hpp"

struct Scene_msgbox_enter::Impl {
  constx int WND_X = 375;
  constx int WND_Y = 110;
  utf32 m_msg {};
  utf32 m_title {};
  Action m_action {};
  Image m_bg {};
  bool _replay_read_mode {};
  bool _enable_replay {};

  inline Impl(cr<utf32> msg, cr<utf32> title, cr<Action> action)
  : m_msg {msg}
  , m_title {title}
  , m_action {action}
  {
    // забэкапить режим воспроизведения реплея, чтобы подождать выхода из этого окна
    _replay_read_mode = hpw::replay_read_mode;
    _enable_replay = hpw::enable_replay;
    hpw::replay_read_mode = false;
    hpw::enable_replay = false;
    make_bg();
  }

  ~Impl() {
    hpw::replay_read_mode = _replay_read_mode;
    hpw::enable_replay = _enable_replay;
  }

  inline void update(const Delta_time dt) {
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
    hpw_blur(for_blur, Image(for_blur), 5);
    insert(m_bg, for_blur, window.pos);

    // нарисовать окно с рамкой
    draw_rect_filled<&blend_alpha>(m_bg, window, Pal8::black, 200);
    draw_rect(m_bg, border, Pal8::gray);

    // текст сообщений
    graphic::font->draw(m_bg, window.pos + Vec(15, 10), m_title, &blend_avr);
    text_bordered(m_bg, m_msg, graphic::font.get(), window, {15, 28});
  } // make_bg
}; // Impl

Scene_msgbox_enter::Scene_msgbox_enter(cr<utf32> msg, cr<utf32> title, cr<Action> action)
  : impl {new_unique<Impl>(msg, title, action)} {}
Scene_msgbox_enter::~Scene_msgbox_enter() {}
void Scene_msgbox_enter::update(const Delta_time dt) { impl->update(dt); }
void Scene_msgbox_enter::draw(Image& dst) const { impl->draw(dst); }
