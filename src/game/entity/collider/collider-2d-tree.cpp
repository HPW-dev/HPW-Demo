#include <omp.h>
#include <cassert>
#include "collider-2d-tree.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

struct Collider_2d_tree::Impl {
  using Collidables = Vector<Collidable*>;
  uint m_depth {};

  explicit inline Impl(uint depth)
  : m_depth {depth} {
    assert(m_depth > 6);
  }

  inline static void test_collide(Collidable& a, Collidable& b) {
    return_if (!a.collision_possible(b));
    return_if (!a.hitbox_test(b));
    a.collide_with(b);
  }

  // получить список объектов доступных для столкновения
  inline static Collidables collidable_filter(CN<Entities> entities) {
    Collidables ret;
    for (cnauto ent: entities) {
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

  
  inline void operator()(CN<Entities> entities, Delta_time dt) {
    auto collidables = collidable_filter(entities);
    make_tree(collidables);
    process_collisions();
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    
  }

  inline void make_tree(CN<Collidables> entities) {

  }

  inline void process_collisions() {

  }
}; // Impl

Collider_2d_tree::Collider_2d_tree(uint depth): impl {new_unique<Impl>(depth)} {}
Collider_2d_tree::~Collider_2d_tree() {}
void Collider_2d_tree::operator()(CN<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_2d_tree::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
