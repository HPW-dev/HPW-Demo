#include "shaker.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image.hpp"

namespace epge {

enum class Blend_id: int {PAST = 0, AVR, AVR_MAX, MIN, MAX, DIFF, AND, OR, XOR, MAX_VALUE};

struct Shaker::Impl final {
  double _offset {2}; // как далеко должен смещаться кадр
  int _blend_mode {scast<int>(Blend_id::PAST)};

  inline Str name() const noexcept { return "shaker"; }
  inline Str desc() const noexcept { return "causes frame shaking"; }

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

  inline void draw(Image& dst) const noexcept {
    
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_double>("offset", "how far should the frame move", _offset, 0.1, 20, 0.1, 1),
      new_shared<epge::Param_int>("blend mode", "pixel blending mode: "
        "0-copy-paste; 1-avr; 2-avr-max, 3-min; 4-max; 5-difference; 6-and; 7-or; 8-xor",
        _blend_mode, 0, scast<int>(Blend_id::MAX_VALUE-1), 1, 1),
    };
  }
}; // Impl

Shaker::Shaker(): impl{new_unique<Impl>()} {}
Shaker::~Shaker() {}
Str Shaker::name() const noexcept { return impl->name(); }
Str Shaker::desc() const noexcept { return impl->desc(); }
void Shaker::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Shaker::params() noexcept { return impl->params(); }

} // epge ns
