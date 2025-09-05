#include <cassert>
#include "epge-interlacer.hpp"
#include "graphic/image/image.hpp"
#include "game/core/canvas.hpp"
#include "game/util/locale.hpp"
#include "graphic/effect/interlacer.hpp"

namespace epge {

struct Interlacer::Impl final {
  int _lx {0};
  mutable int _ly {1};
  mutable ::Interlacer _frame_maker {0, 1, scast<uint>(graphic::width), scast<uint>(graphic::height)};
  mutable int _old_lx {};
  mutable int _old_ly {};

  inline Str name() const noexcept { return "interlacer"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.interlacer." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const noexcept { return LOCSTR("desc"); }
  
  inline bool _check_values() const noexcept {
    if (_lx == _ly && _lx == 0)
      _ly = 1;
    const bool ret = (_old_lx != _lx) || (_old_ly != _ly);
    _old_lx = _lx;
    _old_ly = _ly;
    return ret;
  }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    if (_check_values())
      _frame_maker = ::Interlacer(_lx, _ly, dst.X, dst.Y);
    _frame_maker(dst);
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("lines", "number of lines to skip", _lx, 0, 8, 1, 2),
      new_shared<Param_int>("column", "number of columns to skip", _ly, 0, 8, 1, 2),
    };
  }
}; // Impl

EPGE_IMPL_MAKER(Interlacer)

} // epge ns
