#include <omp.h>
#include <cassert>
#include "collider-simple.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"

struct Collider_simple::Impl {
  using Collidables = Vector<Collidable*>;

  inline static void test_collide(Collidable& a, Collidable& b) {
    return_if (!a.collision_possible(b));
    return_if (!a.hitbox_test(b));
    a.collide_with(b);
  }

  // получить список объектов доступных для столкновения
  inline Collidables collidable_filter(cr<Entities> entities) const {
    Collidables ret;
    for (crauto ent: entities) {
      assert(ent);
      cont_if (!ent->status.live);
      cont_if (!ent->status.collidable);
      // при соблюдении условия выше можно смело кастовать
      auto collidable = ptr2ptr<Collidable*>(ent.get());
      cont_if (!collidable->get_hitbox());
      ret.push_back(collidable);
    }
    return ret;
  }

  inline void operator()(cr<Entities> entities, Delta_time dt) {
    auto collidables = collidable_filter(entities);
    cauto entitys_sz = collidables.size();
    return_if(entitys_sz <= 1); // защита от зацикливания

    // сделать список уникальных пар для проверки столкновений
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*collidables[a_i], *collidables[b_i]);
  }
}; // Impl

Collider_simple::Collider_simple(): impl {new_unique<Impl>()} {}
Collider_simple::~Collider_simple() {}
void Collider_simple::operator()(cr<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_simple::debug_draw(Image& dst, const Vec camera_offset) { /*impl->debug_draw(dst, camera_offset);*/ }
