#include <cassert>
#include "mirror.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/rotation.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/util/locale.hpp"

namespace epge {

struct Mirror::Impl {
  bool _mirror_v {true};
  bool _mirror_h {false};
  bool _rotate_180 {false};

  inline Str name() const { return "mirror"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.mirror." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline void draw(Image& dst) const {
    assert(dst);

    if (_mirror_v)
      insert_fast(dst, mirror_v(dst));

    if (_mirror_h)
      insert_fast(dst, mirror_h(dst));

    if (_rotate_180)
      insert_fast(dst, rotate_180(dst));
  }

  inline Params params() {
    return Params {
      new_shared<Param_bool>("vertical", U"vertical", U"mirrors the game frame vertically", _mirror_v),
      new_shared<Param_bool>("horizontal", U"horizontal", U"mirrors the game frame horizontally", _mirror_h),
      new_shared<Param_bool>("rotate", U"rotate", U"rotate the game frame by 180 degrees", _rotate_180),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Mirror)

} // epge ns
