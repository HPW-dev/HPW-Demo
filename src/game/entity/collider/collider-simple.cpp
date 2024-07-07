#include <omp.h>
#include "collider-simple.hpp"
#include "game/core/debug.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

void Collider_simple::test_collide(Entity& a, Entity& b) {
  // проверить что объекты живые
  return_if(!a.status.live);
  return_if(!b.status.live);
  // проверить что можно сталкиваться
  return_if (!a.status.collidable || !b.status.collidable);
  // сталкиваемые объекты можно смело кастовать
  nauto a_collidable = *( ptr2ptr<Collidable*>(&a) );
  nauto b_collidable = *( ptr2ptr<Collidable*>(&b) );
  a_collidable.resolve_collision(b_collidable);
}

void Collider_simple::operator()(CN<Entities> entities, Delta_time dt) {
  cauto entitys_sz = entities.size();
  return_if(entitys_sz <= 1); // защита от зацикливания

  // проверить столкновения пар объектов (без повторений)
  #pragma omp parallel for simd schedule(dynamic, 4)
  for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
  for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
    test_collide(*entities[a_i], *entities[b_i]);
}
