#include <cassert>
#include "anim-info.hpp"
#include "game/core/anims.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"

void Anim_info::load(CN<Yaml> node) {
  if ( !node.check()) {
    detailed_log("пустая нода с анимацией, выход из функции");
    return;
  }
  
  auto anim_name = node.get_str("name");
  try {
    anim = hpw::anim_mgr->find_anim(anim_name).get();
  } catch (...) {
    detailed_log("нет анимации с именем\"" << anim_name << "\"\n");
  }

  fixed_deg           = node.get_bool  ("fixed_deg");
  default_deg         = node.get_real  ("default_deg");
  return_back         = node.get_bool  ("return_back");
  rand_cur_frame      = node.get_bool  ("rand_cur_frame");
  speed_scale_minmax  = node.get_v_real("anim_speed_scale");
  layer_up            = node.get_bool  ("layer_up", false);
  ignore_scatter      = node.get_bool  ("ignore_scatter");
  disable_motion      = node.get_bool  ("disable_motion");
  
  // читать пиксель блендинг
  if (auto blend_f_name = node.get_str("blend_f"); !blend_f_name.empty())
    bf = find_blend_f(blend_f_name);

  // заюзать контур, если есть
  if (auto contour_bf_name = node.get_str("contour_bf");
  !contour_bf_name.empty() && !anim_name.empty()) {
    light_mask_anim = make_light_mask(anim_name, anim_name + ".light_mask").get();
    contour_bf = find_blend_f(contour_bf_name);
  }
} // load

void Anim_info::accept(Entity& dst) {
  if (anim)
    dst.anim_ctx.set_anim(anim);
  dst.anim_ctx.blend_f = bf;
  dst.anim_ctx.set_default_deg(default_deg);
  dst.status.fixed_deg = fixed_deg;
  dst.status.return_back = return_back;
  dst.status.ignore_scatter = ignore_scatter;
  dst.status.disable_motion = disable_motion;
  dst.status.layer_up = layer_up;

  if (light_mask_anim) {
    dst.anim_ctx.set_contour(light_mask_anim);
    dst.anim_ctx.contour_bf = contour_bf;
  }
  if (rand_cur_frame)
    dst.anim_ctx.randomize_cur_frame_safe();
  if (speed_scale_minmax.size() > 1) {
    dst.anim_ctx.set_speed_scale( rndr(
      speed_scale_minmax[0],
      speed_scale_minmax[1]
    ) );
  }
} // accept
