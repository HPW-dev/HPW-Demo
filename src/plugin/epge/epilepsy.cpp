#include <omp.h>
#include <cassert>
#include "epilepsy.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Epilepsy::Impl final {
  enum class Mode {
    _xor = 0,
    _and,
    _or,
    _plus,
    _mul,
    _max,
    _min,
    _avr,
    MAX,
  };

  int _mode {scast<int>(Mode::_or)};

  inline Str name() const noexcept { return "epilepsy"; }
  inline Str desc() const noexcept { return "various color rotations"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    cauto rnd = rndu_fast() % 256u;

    switch (scast<Mode>(_mode)) {
      default:
      case Mode::_xor:  xor_blend  (dst, rnd); break;
      case Mode::_and:  and_blend  (dst, rnd); break;
      case Mode::_or:   or_blend   (dst, rnd); break;
      case Mode::_plus: plus_blend (dst, rnd); break;
      case Mode::_mul:  mul_blend  (dst, rnd); break;
      case Mode::_max:  max_blend  (dst, rnd); break;
      case Mode::_min:  min_blend  (dst, rnd); break;
      case Mode::_avr:  avr_blend  (dst, rnd); break;
    }
  }

  inline void xor_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val ^= rnd;
  }

  inline void and_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val &= rnd;
  }

  inline void or_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val |= rnd;
  }

  inline void plus_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val += rnd;
  }

  inline void mul_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val *= rnd;
  }

  inline void max_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = std::max<unsigned>(dst[i].val, rnd);
  }

  inline void min_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = std::min<unsigned>(dst[i].val, rnd);
  }

  inline void avr_blend(Image& dst, const unsigned rnd) const noexcept {
    #pragma omp parallel for simd if (dst.size > 64 * 64)
    cfor (i, dst.size)
      dst[i].val = (dst[i].val + rnd) >> 1;
  }
  
  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("mode", "pixel blending modes:\n"
        "  0 - XOR, 1 - &, 2 - |, 3 - +,\n"
        "  4 - *,   5 - >, 6 - <, 7 - average", _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
    };
  }
}; // Impl

Epilepsy::Epilepsy(): impl{new_unique<Impl>()} {}
Epilepsy::~Epilepsy() {}
Str Epilepsy::name() const noexcept { return impl->name(); }
Str Epilepsy::desc() const noexcept { return impl->desc(); }
void Epilepsy::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Epilepsy::params() noexcept { return impl->params(); }

} // epge ns
