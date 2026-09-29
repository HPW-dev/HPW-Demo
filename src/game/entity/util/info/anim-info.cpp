#include "pch.hpp"
#include "anim-info.hpp"
#include "game/core/anims.hpp"
#include "game/util/anim-helper.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "engine/graphic/util/graphic-util.hpp"

void Anim_info::load(cr<nlohmann::json> node) {
  if (!node || node.empty()) {
    log_debug << "пустая нода с анимацией, выход из функции";
    return;
  }
  
  auto anim_name = node["name"].get<Str>();
  try {
    anim = hpw::anim_mgr->find_anim(anim_name).get();
  } catch (...) {
    log_debug << "нет анимации с именем\"" << anim_name << "\"";
  }

  fixed_deg           = node["fixed_deg"].get<bool>();
  default_deg         = node["default_deg"].get<real>();
  return_back         = node["return_back"].get<bool>();
  rand_cur_frame      = node["rand_cur_frame"].get<bool>();
  speed_scale_minmax  = node["anim_speed_scale"].get<Vector<real>>();
  layer_up            = node.value<bool>("layer_up", false);
  ignore_scatter      = node["ignore_scatter"].get<bool>();
  disable_motion      = node["disable_motion"].get<bool>();
  
  // читать пиксель блендинг
  if (auto blend_f_name = node["blend_f"].get<Str>(); !blend_f_name.empty())
    bf = find_blend_f(blend_f_name);

  // заюзать контур, если есть
  if (auto contour_bf_name = node["contour_bf"].get<Str>();
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
