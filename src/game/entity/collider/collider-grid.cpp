#include <omp.h>
#include <cassert>
#include "collider-grid.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"

struct Collider_grid::Impl {
  using Collidables = Vector<Collidable*>;
  using List = Vector<Collidable*>;
  int grid_sz = 10; // размер ячейки сетки в пикселях
  int grid_mx {}; // ширина сетки
  int grid_my {}; // высота сетки
  Vec grid_offset {}; // левый верхний угол сетки
  Vector<List> grid {}; // сетка со списками объектов

  inline static void test_collide(Collidable& a, Collidable& b) {
    return_if (!a.collision_possible(b));
    return_if (!a.hitbox_test(b));
    a.collide_with(b);
  }

  // получить список объектов доступных для столкновения
  inline Collidables collidable_filter(CN<Entities> entities) const {
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

  // подготовить сетку
  inline void config_grid(CN<Collidables> entities) {

  }

  // добавить объект на сетку
  inline void insert(CP<Collidable> entity) {

  }

  // найти столкновения в сетке
  inline void process_collisions() {

  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {

  }

  inline void operator()(CN<Entities> entities, Delta_time dt) {
    auto collidables = collidable_filter(entities);
    config_grid(collidables);
    for (cnauto entity: collidables)
      insert(entity);
    process_collisions();
    
    /*
    cauto entitys_sz = collidables.size();
    return_if(entitys_sz <= 1); // защита от зацикливания

    // сделать список уникальных пар для проверки столкновений
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*collidables[a_i], *collidables[b_i]);
    */
  }
}; // Impl

Collider_grid::Collider_grid(): impl {new_unique<Impl>()} {}
Collider_grid::~Collider_grid() {}
void Collider_grid::operator()(CN<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_grid::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
