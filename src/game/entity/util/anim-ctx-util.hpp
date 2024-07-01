#pragma once
#include "util/math/num-types.hpp"
#include "util/mempool.hpp"

class Anim_ctx;
class Entity;
class Hitbox;

namespace anim_ctx_util {
// получить хитбокс анимации
CP<Hitbox> get_hitbox(CN<Anim_ctx> anim_ctx, real degree, CN<Entity> entity);
// назначить новых хитбокс
void update_hitbox(Anim_ctx& anim_ctx, CN<Pool_ptr(Hitbox)> hitbox);
}
