#pragma once
#include "util/math/num-types.hpp"
#include "util/mempool.hpp"

class Anim_ctx;
class Entity;
class Hitbox;

namespace anim_ctx_util {
// получить хитбокс анимации
cp<Hitbox> get_hitbox(cr<Anim_ctx> anim_ctx, real degree, cr<Entity> entity);
// назначить новых хитбокс
void update_hitbox(Anim_ctx& anim_ctx, cr<Pool_ptr(Hitbox)> hitbox);
}
