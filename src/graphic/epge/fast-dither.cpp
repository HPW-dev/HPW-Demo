#include <cassert>
#include <array>
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"

EPGE_CLASS_BEGIN(fast_dither)
  int _mode = 1;
  std::array<int, 256> _pal8_to_bw_i256 {};
  std::array<int, 256> _pal8_to_i256 {};
  std::array<Pal8, 256> _i256_to_pal8 {};
  mutable bool _flicker = false;

public:
  Epge_fast_dither() {
    // заполнить таблицы
    assert(_pal8_to_i256.size() == _i256_to_pal8.size());
    cfor (i, _pal8_to_i256.size()) {
      cauto ui = scast<std::size_t>(i);
      cauto ui8 = scast<std::uint8_t>(i);
      _pal8_to_bw_i256[ui] = Pal8{ui8}.to_real() > 0.5 ? 255 : 0;
      _pal8_to_i256[ui] = Pal8{ui8}.to_real() * 255.0;
      _i256_to_pal8[ui] = Pal8::from_real(i / 255.0);
    }
  }

  void draw_error_diff(Image& dst) const {
    cfor (i, dst.size - 1) {
      cauto bw = _pal8_to_bw_i256[dst[i].val];
      cauto diff = _pal8_to_i256[dst[i].val] - bw;
      dst[i] = _i256_to_pal8[bw];
      cauto idx = _pal8_to_i256[dst[i+1].val] + diff;
      dst[i + 1] = _i256_to_pal8[std::clamp<int>(idx, 0, 255)];
    }
  }

  void draw_error_diff_flicker(Image& dst) const {
    _flicker = !_flicker;
    cauto mul = _flicker ? -1 : 1;

    cfor (i, dst.size - 1) {
      cauto bw = _pal8_to_bw_i256[dst[i].val];
      cauto diff = _pal8_to_i256[dst[i].val] - bw * mul;
      dst[i] = _i256_to_pal8[bw];
      cauto idx = _pal8_to_i256[dst[i+1].val] + diff * mul;
      dst[i + 1] = _i256_to_pal8[std::clamp<int>(idx, 0, 255)];
    }
  }

  void draw_error_diff_fast(Image& dst) const {
    cfor (i, dst.size - 1) {
      cauto bw = dst[i].val > 127 ? 255 : 0;
      cauto diff = dst[i].val - bw;
      dst[i] = bw;
      dst[i + 1] += diff;
    }
  }

  void draw(Image& dst) const {
    assert(dst);

    switch (_mode) {
      default:
      case 0: draw_error_diff_fast(dst); break;
      case 1: draw_error_diff(dst); break;
      case 2: draw_error_diff_flicker(dst); break;
    }
  }

  Params params() { return Params {
    EPGE_PARAM_INT(mode, fast_dither, _mode, 0, 2, 1, 1)
  }; }
EPGE_CLASS_END(fast_dither)
