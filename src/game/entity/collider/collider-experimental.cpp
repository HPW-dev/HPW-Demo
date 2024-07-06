#include <omp.h>
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

  inline void operator()(CN<Entities> entities, Delta_time dt) {
    /*cauto entitys_sz = entities.size();
    return_if(entitys_sz <= 1); // защита от зацикливания

    // проверить столкновения пар объектов (без повторений)
    #pragma omp parallel for simd schedule(dynamic, 4)
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*entities[a_i], *entities[b_i]);*/

    m_areas.clear();
    for (nauto ent: entities)
      insert(ent.get());
    for (nauto area: m_areas)
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
    for (nauto area: m_areas)
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
  inline Rect make_rect(CN<Entity> ent) const {
    cauto hitbox = ent.get_hitbox();
    assert(hitbox);
    cauto sz = hitbox->simple.r * 2;
    return Rect {
      // TODO влияние офсета не протещено
      ent.phys.get_pos() - Vec(sz/2., sz/2.),
      Vec(sz, sz)
    };
  }

  inline static Rect expand_rect(CN<Rect> a, CN<Rect> b) {
    return a;
  }

  // проверить столкновения в области
  inline void process_collisions(CN<Area> area) {

  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    for (cnauto area: m_areas) {
      auto rect = area.rect;
      rect.pos += camera_offset;
      draw_rect<&blend_diff>(dst, rect, Pal8::white);
    }
  }
}; // Impl

Collider_experimental::Collider_experimental(): impl {new_unique<Impl>()} {}
Collider_experimental::~Collider_experimental() {}
void Collider_experimental::operator()(CN<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_experimental::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
