#include <omp.h>
#include <utility>
#include <algorithm>
#include "collider-experimental.hpp"
#include "game/core/debug.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

struct Collider_experimental::Impl {
  using List = Vector<Entity*>;

  // область, в которую входят объекты
  struct Area {
    Rect rect {};
    List entities {};
  };
  Vector<Area> m_areas {};

  inline void test_collide(Entity& a, Entity& b) {
    // столкновение с собой не проверять
    return_if(std::addressof(a) == std::addressof(b));
    // проверить что объекты живые
    return_if(!a.status.live);
    return_if(!b.status.live);
    // проверить что можно сталкиваться
    return_if (!a.status.collidable || !b.status.collidable);

    // сталкиваемые объекты можно смело кастовать
    rauto a_collidable = *( ptr2ptr<Collidable*>(&a) );
    rauto b_collidable = *( ptr2ptr<Collidable*>(&b) );
    a_collidable.resolve_collision(b_collidable);
  }

  inline void operator()(cr<Entities> entities, Delta_time dt) {
    m_areas.clear();
    for (rauto ent: entities)
      insert(ent.get());
    for (rauto area: m_areas)
      process_collisions(area);
  }

  // добавить объект в области
  inline void insert(Entity* ent) {
    // проверить что объекту можно сталкиваться
    return_if(!ent->status.live);
    return_if(!ent->status.collidable);

    cauto rect = make_rect(*ent);

    // найти столкновения с другими областями
    bool intersected {};
    for (rauto area: m_areas)
      if (intersect(area.rect, rect)) {
        area.entities.push_back(ent);
        // расширить область чтобы она покрывала два объекта
        area.rect = expand_rect(area.rect, rect);
        intersected = true;
      }
    
    // когда нету никаких областей или не попали никуда
    if (!intersected) {
      Area area {.rect = rect, .entities = {ent}};
      m_areas.emplace_back( std::move(area) );
    }
  } // insert

  // создаёт прямоугольник оборачивающий хитбокс
  inline Rect make_rect(cr<Entity> ent) const {
    cauto hitbox = ent.get_hitbox();
    assert(hitbox);
    cauto sz = hitbox->simple.r * 2;
    return Rect {
      ent.phys.get_pos() - Vec(sz/2., sz/2.),
      Vec(sz, sz)
    };
  }

  inline static Rect expand_rect(cr<Rect> a, cr<Rect> b) {
    Rect ret;
    ret.pos.x = std::min(a.pos.x, b.pos.x);
    ret.pos.y = std::min(a.pos.y, b.pos.y);
    // крайние точки
    Vec a_end = a.pos + a.size;
    Vec b_end = b.pos + b.size;
    Vec end;
    end.x = std::max(a_end.x, b_end.x);
    end.y = std::max(a_end.y, b_end.y);
    ret.size = end - ret.pos;
    return ret;
  }

  // проверить столкновения в области
  inline void process_collisions(cr<Area> area) {
    cauto entitys_sz = area.entities.size();
    return_if(entitys_sz <= 1);

    // проверить столкновения пар объектов (без повторений)
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*area.entities[a_i], *area.entities[b_i]);
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    for (crauto area: m_areas) {
      auto rect = area.rect;
      rect.pos += camera_offset;
      draw_rect<&blend_diff>(dst, rect, Pal8::white);
    }
  }
}; // Impl

Collider_experimental::Collider_experimental(): impl {new_unique<Impl>()} {}
Collider_experimental::~Collider_experimental() {}
void Collider_experimental::operator()(cr<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_experimental::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
