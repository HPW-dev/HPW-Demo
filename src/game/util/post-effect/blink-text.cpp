#include <cassert>
#include "blink-text.hpp"
#include "util/math/timer.hpp"
#include "util/math/rect.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/fonts.hpp"
#include "game/util/sync.hpp"

struct Blink_text::Impl {
  mutable Timer m_timeout {};
  utf32 m_txt {};

  inline Impl(const Delta_time timeout, CN<utf32> txt)
  : m_timeout {timeout}
  , m_txt {txt}
  {
    assert(timeout > 0);
    assert( !txt.empty());
  }

  inline bool update(const Delta_time dt) { return m_timeout.update(dt); }

  inline void draw(Image& dst) const {
    // текст мигает
    return_if ((graphic::frame_count % 70) > 35);

    // текст в чёрном прямоугольнике
    const Rect rect(Vec{}, graphic::font->text_size(m_txt) + Vec(6, 6));
    Image rect_with_txt(rect.size.x, rect.size.y, Pal8::black);
    graphic::font->draw(rect_with_txt, Vec(3, 3), m_txt);

    // нарисовать в центре экрана
    insert(dst, rect_with_txt, {
      (dst.X - rect.size.x) / 2.0,
      ((dst.Y - rect.size.y) / 5.0) * 3.0
    });
  }
}; // impl

Blink_text::~Blink_text() {}
Blink_text::Blink_text(const Delta_time timeout, CN<utf32> txt): impl { new_unique<Impl>(timeout, txt) } {}
bool Blink_text::update(const Delta_time dt) { return impl->update(dt); }
void Blink_text::draw(Image& dst) const { impl->draw(dst); }
