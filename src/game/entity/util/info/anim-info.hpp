#pragma once
#include "graphic/animation/anim.hpp"
#include "graphic/image/color-blend.hpp"
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"

class Yaml;
class Entity;

// Инфа о анимации для загрузчика entity
struct Anim_info {
  Anim* anim {}; // ссыль на анимацию из банка
  Anim* light_mask_anim {}; // ссыль анимацию контура
  blend_pf bf {&blend_past}; // стиль наложения анимации
  blend_pf contour_bf {&blend_past}; // эффект смешивания контура
  real default_deg {};
  bool layer_up {};
  bool fixed_deg {};
  bool return_back {}; // проиграть обратно при конце анимации
  bool rand_cur_frame {}; // начальный фрейм будет случайным
  Vector<real> speed_scale_minmax {}; // если задан, то укажет диапазон скоростей анимации
  bool ignore_scatter {};
  bool disable_motion {};

  void load(CN<Yaml> node);
  void accept(Entity& dst);
};
