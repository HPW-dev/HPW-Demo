#include "pch.hpp"
#include "epge.hpp"
#include "engine/graphic/effect/dither.hpp"

EPGE_CLASS_BEGIN(dither_blue_noise)
  int _mode = 1;
  bool _use_gamma_corr = true;
  bool _use_jitter = true;

public:
  void draw(Image& dst) const {
    cauto mode = scast<Dither_blue_noise_mode>(_mode);
    dither_blue_noise(dst, mode, _use_gamma_corr, _use_jitter);
  }

  Params params() { return Params {
    EPGE_PARAM_INT(mode, dither_blue_noise, _mode, 0, 2, 1, 1)
    EPGE_PARAM_BOOL(use_gamma_corr, dither_blue_noise, _use_gamma_corr)
    EPGE_PARAM_BOOL(use_jitter, dither_blue_noise, _use_jitter)
  }; }
EPGE_CLASS_END(dither_blue_noise)
