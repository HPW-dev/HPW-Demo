#include <omp.h>
#include <cassert>
#include <cmath>
#include "collider-grid.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/log.hpp"

struct Collider_grid::Impl {
  using Collidables = Vector<Collidable*>;
  using List = Vector<Collidable*>;
  int m_grid_sz = 20; // размер ячейки сетки в пикселях
  int m_grid_mx {}; // ширина сетки
  int m_grid_my {}; // высота сетки
  Vec m_grid_offset {}; // левый верхний угол сетки
  Vector<List> m_grid {}; // сетка со списками объектов

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

  inline void clear_grid() {
    m_grid.clear();
    m_grid_offset = {};
    m_grid_mx = m_grid_my = {};
  }

  // подготовить сетку
  inline void config_grid(CN<Collidables> entities) {
    clear_grid();
    return_if(entities.size() < 2);

    // найти крайние точки слева сверху и справа снизу
    Vec lu { 1'000'000,  1'000'000};
    Vec rd {-1'000'000, -1'000'000};
    for (cnauto ent: entities) {
      cauto pos = ent->phys.get_pos();
      lu.x = std::min(lu.x, pos.x);
      lu.y = std::min(lu.y, pos.y);
      rd.x = std::max(rd.x, pos.x);
      rd.y = std::max(rd.y, pos.y);
    }

    m_grid_offset = lu;
    m_grid_mx = std::ceil((rd.x - lu.x) / m_grid_sz);
    m_grid_my = std::ceil((rd.y - lu.y) / m_grid_sz);
    m_grid.resize(m_grid_mx * m_grid_my);
  }

  // добавить объект на сетку
  inline void insert(CP<Collidable> entity) {

  }

  // найти столкновения в сетке
  inline void process_collisions() {
    /*
    // сделать список уникальных пар для проверки столкновений
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*collidables[a_i], *collidables[b_i]);
    */
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    return_if(m_grid.size() < 2);
    assert(m_grid_mx > 0);
    assert(m_grid_my > 0);
    // нарисовать сетку
    int offset_x = std::floor(m_grid_offset.x + camera_offset.x);
    int offset_y = std::floor(m_grid_offset.y + camera_offset.y);
    cfor (y, m_grid_my) {
      const Vec a(offset_x, offset_y + y * m_grid_sz);
      const Vec b(offset_x + m_grid_mx * m_grid_sz, offset_y + y * m_grid_sz);
      draw_line<&blend_diff>(dst, a, b, Pal8::white);
    }
    cfor (x, m_grid_mx) {
      const Vec a(offset_x + x * m_grid_sz, offset_y);
      const Vec b(offset_x + x * m_grid_sz, offset_y + m_grid_my * m_grid_sz);
      draw_line<&blend_diff>(dst, a, b, Pal8::white);
    }
  } // debug_draw

  inline void operator()(CN<Entities> entities, Delta_time dt) {
    auto collidables = collidable_filter(entities);
    hpw_log("collidables: " << collidables.size() << '\n');
    config_grid(collidables);
    for (cnauto entity: collidables)
      insert(entity);
    process_collisions();
  }
}; // Impl

Collider_grid::Collider_grid(): impl {new_unique<Impl>()} {}
Collider_grid::~Collider_grid() {}
void Collider_grid::operator()(CN<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_grid::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
