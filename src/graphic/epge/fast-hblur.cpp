#include <cassert>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"

namespace epge {

#define LOCSTR(NAME) get_locale_str("epge.effect.fast_hblur." NAME)

class Fast_hblur: public epge::Base {
public:
  Str name() const { return "fast horizontal blur"; }
  utf32 localized_name() const { return LOCSTR("name"); }
  utf32 desc() const { return LOCSTR("desc"); }

  void draw(Image& dst) const {
    assert(dst);
  }

  Params params() {
    return {};
  }
}; // class

inline Epge_registrator<Fast_hblur> _ignore_Fast_hblur {};

} // epge ms
