#include <omp.h>
#include <cmath>
#include <cassert>
#include "color-rotation.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Color_rotation::Impl final {
  enum class Mode {
    _xor = 0,
    _and,
    _or,
    _mul,
    _max,
    _min,
    _avr,
    MAX,
  };

  int _mode {scast<int>(Mode::_xor)};
  Delta_time _speed {23};
  Delta_time _state {};

  inline Str name() const noexcept { return "color rotation"; }
  inline Str desc() const noexcept { return "twists colors in a circle"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);

    switch (scast<Mode>(_mode)) {
      default:
      case Mode::_xor:  xor_blend (dst); break;
      case Mode::_and:  and_blend (dst); break;
      case Mode::_or:   or_blend  (dst); break;
      case Mode::_mul:  mul_blend (dst); break;
      case Mode::_max:  max_blend (dst); break;
      case Mode::_min:  min_blend (dst); break;
      case Mode::_avr:  avr_blend (dst); break;
    }
  }

  inline void xor_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val ^= state;
  }

  inline void and_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val &= state;
  }

  inline void or_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val |= state;
  }

  inline void mul_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val *= state;
  }

  inline void max_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = std::max<unsigned>(dst[i].val, state);
  }

  inline void min_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = std::min<unsigned>(dst[i].val, state);
  }

  inline void avr_blend(Image& dst) const noexcept {
    const int state = std::round(_state);
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = (dst[i].val + state) >> 1;
  }
  
  inline Params params() noexcept {
    return Params {
      new_shared<Param_int>("mode", "pixel blending modes:\n"
        "  0 - XOR, 1 - &, 2 - |, 3 - *,\n"
        "  4 - >,   5 - <, 6 - average", _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
      new_shared<Param_double>("speed", "color flicker speed", _speed, 0.1, 255, 0.1, 1),
    };
  }

  inline void update(const Delta_time dt) noexcept {
    _state += dt * _speed * 255.0 * 60.0;
    _state = std::fmod(_state, 255.0);
  }
}; // Impl

Color_rotation::Color_rotation(): impl{new_unique<Impl>()} {}
Color_rotation::~Color_rotation() {}
Str Color_rotation::name() const noexcept { return impl->name(); }
Str Color_rotation::desc() const noexcept { return impl->desc(); }
void Color_rotation::update(const Delta_time dt) noexcept { impl->update(dt); }
void Color_rotation::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Color_rotation::params() noexcept { return impl->params(); }

} // epge ns
