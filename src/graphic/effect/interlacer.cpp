#include <cassert>
#include "interlacer.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

struct Interlacer::Impl {
  uint mx {}, my {}; // размер обрабатываемого блока пикселей
  mutable uint ix {}, iy {}; // индексы бегают по блоку
  mutable Image screen {}; // внутренний буффер накопления

  inline Impl(uint _mx, uint _my, uint screen_w, uint screen_h)
  : mx {_mx + 1}
  , my {_my + 1}
  , screen (screen_w, screen_h, Pal8::black)
  {
    assert(mx >= 0);
    assert(my >= 0);
    assert(screen);
  }

  inline void draw(Image& dst) const {
    // такой варик игнорить
    return_if (mx == 1 && my == 1);
    // созданный буффер должен совпадать с dst
    assert(dst.X == screen.X);
    assert(dst.Y == screen.Y);

    // вставить новые пиксели только по индексам ix/iy
    cfor (y, dst.Y)
    cfor (x, dst.X)
      if ((x % mx) == ix && (y % my) == iy)
        screen(x, y) = dst(x, y);

    // сдвинуть индексы вставки
    if (++ix >= mx) {
      ix = 0;
      iy = (iy + 1) % my;
    }

    insert_fast(dst, screen);
  } // draw()
}; // Interlacer::impl

Interlacer::Interlacer(uint mx, uint my, uint screen_w, uint screen_h)
: impl {new_shared<Impl>(mx, my, screen_w, screen_h)}
{}

Interlacer::Interlacer(const Interlacer& other) {
  if (this != std::addressof(other))
    impl = other.impl;
  assert(impl);
}

void Interlacer::operator()(Image& dst) const { impl->draw(dst); }
