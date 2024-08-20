#include "anim-ctx-util.hpp"
#include "hitbox.hpp"
#include "anim-ctx.hpp"
#include "game/entity/entity.hpp"
#include "graphic/animation/anim.hpp"
#include "util/error.hpp"

namespace anim_ctx_util {

cp<Hitbox> get_hitbox(cr<Anim_ctx> anim_ctx,
real degree, cr<Entity> entity) {
  cauto anim = anim_ctx.get_anim();
  return_if( !anim, nullptr);
  cauto new_deg = anim_ctx.get_degree_with_flags(degree, entity);
  return anim->get_hitbox(new_deg);
}

void update_hitbox(Anim_ctx& anim_ctx, cr<Pool_ptr(Hitbox)> hitbox) {
  // в режакторе можно менять константные вещи
  auto anim = ccast<Anim*>(anim_ctx.get_anim());
  iferror(!anim, "bad anim ptr");
  anim->update_hitbox(hitbox);
}

} // anim_ctx_util
