#include <omp.h>
#include "collider-experimental.hpp"
#include "game/core/debug.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

struct Collider_experimental::Impl {
  inline void test_collide(Entity& a, Entity& b) {
    // столкновение с собой не проверять
    return_if(std::addressof(a) == std::addressof(b));
    // проверить что объекты живые
    return_if(!a.status.live);
    return_if(!b.status.live);
    // проверить что можно сталкиваться
    return_if (!a.status.collidable || !b.status.collidable);

    // сталкиваемые объекты можно смело кастовать
    nauto a_collidable = *( ptr2ptr<Collidable*>(&a) );
    nauto b_collidable = *( ptr2ptr<Collidable*>(&b) );

    // проверить столкновение
    bool collided = false;
    if (cld_flag_compat(a, b))
      collided = a_collidable.is_collided_with(b_collidable);

    return_if (!collided);
    // нанести урон
    a_collidable.status.collided |= collided;
    a_collidable.sub_hp( b_collidable.get_dmg() );
    b_collidable.status.collided |= collided;
    b_collidable.sub_hp( a_collidable.get_dmg() );
    // TODO вписать инфу о том, с кем столкнулись
    // ...
  }

  inline void operator()(CN<Entitys> entities, Delta_time dt) {
    cauto entitys_sz = entities.size();
    return_if(entitys_sz <= 1); // защита от зацикливания

    // проверить столкновения пар объектов (без повторений)
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*entities[a_i], *entities[b_i]);
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {

  }
}; // Impl

Collider_experimental::Collider_experimental(): impl {new_unique<Impl>()} {}
Collider_experimental::~Collider_experimental() {}
void Collider_experimental::operator()(CN<Entitys> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_experimental::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
