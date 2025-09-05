#include <cassert>
#include "epge-resize.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/canvas.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Resize::Impl {
  enum class Mode {
    neighbor = 0,
    bilinear,
    MAX,
  };

  mutable Image _resized {};
  int _mode {scast<int>(Mode::bilinear)};
  int _w {1};
  int _h {1};
  bool _center {true};
  bool _black_bg {true};

  inline Str name() const noexcept { return "resize"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.resize." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const noexcept { return LOCSTR("desc"); }

  inline Impl() {
    _w = graphic::width;
    _h = graphic::height;
  }

  inline void draw(Image& dst) const noexcept {
    assert(dst);

    // заресайзить
    switch (scast<Mode>(_mode)) {
      default:
      case Mode::neighbor: _resized = resize_neighbor(dst, _w, _h); break;
      case Mode::bilinear: _resized = resize_bilinear(dst, _w, _h); break;
    }

    // центрировать
    Vec pos {};
    if (_center)
      pos = center_point(dst, _resized);

    assert(_resized);
    dst.fill(_black_bg ? Pal8::black : Pal8::white);
    insert(dst, _resized, pos);
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("mode", U"mode", U"resize algorithm:\n"
        U"  0 - neighbor;\n  1 - bilinear.\n", _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
      new_shared<Param_int>("width", U"width", U"screen width (pixels)", _w, 1, graphic::width*2, 1, 4),
      new_shared<Param_int>("height", U"height", U"screen height (pixels)", _h, 1, graphic::height*2, 1, 4),
      new_shared<Param_bool>("center", U"center", U"center the game frame", _center),
      new_shared<Param_bool>("black_bg", U"black bg", U"use black background color", _black_bg),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Resize)

} // epge ns
