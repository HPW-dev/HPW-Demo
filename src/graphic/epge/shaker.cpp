#include "shaker.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/util/vec-helper.hpp"
#include "game/util/locale.hpp"

namespace epge {

enum class Blend_id: int {PAST = 0, AVR, AVR_MAX, MIN, MAX, DIFF, AND, OR, XOR, MAX_VALUE};

struct Shaker::Impl {
  double _offset {2}; // как далеко должен смещаться кадр
  int _blend_mode {scast<int>(Blend_id::PAST)};
  mutable Image _buffer {}; // накладываемый кадр

  inline Str name() const { return "shaker"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.shaker." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  static inline blend_pf find_blend_pf(const Blend_id id) {
    switch (id) {
      default:
      case Blend_id::PAST:    return &blend_past;
      case Blend_id::AVR:     return &blend_avr;
      case Blend_id::AVR_MAX: return &blend_avr_max;
      case Blend_id::MIN:     return &blend_min;
      case Blend_id::MAX:     return &blend_max;
      case Blend_id::DIFF:    return &blend_diff;
      case Blend_id::AND:     return &blend_and;
      case Blend_id::OR:      return &blend_or;
      case Blend_id::XOR:     return &blend_xor;
    }
    return &blend_past;
  }

  inline void draw(Image& dst) const {
    ret_if (_offset <= 0);
    assert(dst);

    cauto ID = scast<Blend_id>(_blend_mode);
    cauto BF = find_blend_pf(ID);
    _buffer = dst;
    cauto pos = get_rand_pos_graphic(-_offset, -_offset, _offset, _offset);
    insert(dst, _buffer, pos, BF, {});
  }

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_double>("offset", U"offset", U"how far should the frame move", _offset, 0.1, 20, 0.1, 0.5),
      new_shared<epge::Param_int>("blend_mode", U"blend mode", U"pixel blending mode: "
        U"0-copy-paste; 1-avr; 2-avr-max, 3-min; 4-max; 5-difference; 6-and; 7-or; 8-xor",
        _blend_mode, 0, scast<int>(Blend_id::MAX_VALUE)-1, 1, 1),
    };
  }

  inline void update(const Delta_time dt) {}
}; // Impl

EPGE_IMPL_MAKER(Shaker)

} // epge ns
