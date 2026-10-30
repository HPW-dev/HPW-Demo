#include "pch.hpp"
#include "epge.hpp"
#include "game/util/locale.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/effect/dither.hpp"

EPGE_CLASS_BEGIN(dither_blue_noise)
  int _mode = 1;

public:
  void draw(Image& dst) const {
    assert(dst);

    switch (_mode) {
      case 0: dither_blue_noise(dst, Dither_blue_noise_mode::fast); break;
      default:
      case 1: dither_blue_noise(dst, Dither_blue_noise_mode::accurate); break;
    }
  }

  Params params() { return Params {
    EPGE_PARAM_INT(mode, dither_blue_noise, _mode, 0, 2, 1, 1)
  }; }
EPGE_CLASS_END(dither_blue_noise)
