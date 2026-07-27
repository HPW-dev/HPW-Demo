#include <omp.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include "collider-grid.hpp"
#include "game/core/fonts.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

struct Collider_grid::Impl {
  using Collidables = Vector<Collidable*>;
  using Sector = Vector<Collidable*>;
  int m_grid_sz {}; // размер ячейки сетки в пикселях
  int m_grid_mx {}; // ширина сетки
  int m_grid_my {}; // высота сетки
  Vec m_grid_offset {}; // левый верхний угол сетки
  Vector<Sector> m_grid {}; // сетка со списками объектов

  explicit inline Impl(int grid_sz)
  : m_grid_sz {grid_sz} {
    assert(m_grid_sz > 6);
  }

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
      cont_if (!bound_check_for_collisions(*collidable));
      cont_if (!collidable->get_hitbox());
      ret.push_back(collidable);
    }
    return ret;
  }

  inline void clear_grid() {
    cfor (y, m_grid_my)
    cfor (x, m_grid_mx) {
      auto sector = get_sector_by_idx(x, y);
      cont_if(!sector);
      sector->clear();
    }
    m_grid_offset = {};
    m_grid_mx = m_grid_my = {};
  }

  // подготовить сетку
  inline void config_grid(cr<Collidables> entities) {
    clear_grid();
    return_if(entities.size() < 2);

    // найти крайние точки слева сверху и справа снизу
    Vec lu { 1'000'000,  1'000'000};
    Vec rd {-1'000'000, -1'000'000};
    for (crauto ent: entities) {
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
  inline void insert(Collidable* entity) {
    // определить размеры хитбокса
    crauto hitbox = entity->get_hitbox();
    cauto pos = entity->phys.get_pos() - m_grid_offset;
    Vec lu = pos + hitbox->simple.offset - hitbox->simple.r;
    lu.x = std::max<real>(0, lu.x);
    lu.y = std::max<real>(0, lu.y);
    const Vec rd = pos + hitbox->simple.offset + hitbox->simple.r + 1;
    
    // залить сектора по всему размеру объекта
    const int mx = std::floor((rd.x - lu.x) / m_grid_sz + 1.75);
    const int my = std::floor((rd.y - lu.y) / m_grid_sz + 1.75);
    cfor (y, my)
    cfor (x, mx) {
      auto sector = get_sector_by_pos(
        lu.x + x * m_grid_sz,
        lu.y + y * m_grid_sz);
      cont_if(!sector);
      sector->push_back(entity);
    }
  } // insert

  // найти столкновения в сетке
  inline void process_collisions() {
    return_if (m_grid_mx == 0);
    return_if (m_grid_my == 0);

    #pragma omp parallel for schedule(dynamic)
    for (crauto sector: m_grid) {
      cauto entitys_sz = sector.size();
      cont_if(entitys_sz < 2);

      if (entitys_sz == 2) {
        test_collide(*sector[0], *sector[1]);
        continue;
      }

      // сделать список уникальных пар для проверки столкновений
      for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
      for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
        test_collide(*sector[a_i], *sector[b_i]);
    }
  }

  inline Sector* get_sector_by_idx(int x, int y) {
    return_if (x < 0, nullptr);
    return_if (x >= m_grid_mx, nullptr);
    return_if (y < 0, nullptr);
    return_if (y >= m_grid_my, nullptr);
    return &m_grid[y * m_grid_mx + x];
  }

  inline Sector* get_sector_by_pos(real x, real y) {
    return get_sector_by_idx(x / m_grid_sz, y / m_grid_sz);
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    return_if (m_grid_mx == 0);
    return_if (m_grid_my == 0);
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
    // написать скольк объектов в ячейке
    cfor (y, m_grid_my)
    cfor (x, m_grid_mx) {
      const Vec pos(
        offset_x + x * m_grid_sz + 2,
        offset_y + y * m_grid_sz + 2);
      crauto sector = get_sector_by_idx(x, y);
      cauto count = sector->size();
      graphic::font->draw(dst, pos, n2s<utf32>(count), &blend_diff);
    }
  } // debug_draw

  inline void operator()(cr<Entities> entities, Delta_time dt) {
    auto collidables = collidable_filter(entities);
    config_grid(collidables);
    for (crauto entity: collidables)
      insert(entity);
    process_collisions();
  }
}; // Impl

Collider_grid::Collider_grid(int grid_sz): impl {new_unique<Impl>(grid_sz)} {}
Collider_grid::~Collider_grid() {}
void Collider_grid::operator()(cr<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_grid::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
