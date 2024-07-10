#include <omp.h>
#include <cassert>
#include "collider-2d-tree.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"

using Collidables = Vector<Collidable*>;

/* ветвь дерева содержит две половины, внутри
которых другие ветви или объекты */
struct Node {
  Rect area {};
  Collidables list {};
  Unique<Node> a {};
  Unique<Node> b {};
  bool is_active {};
  bool x_axis {}; // true - делить по ширине

  // рекурсивно делит пространтство
  inline void split(uint depth) {
    // деление останавливается при лимите depth
    return_if (depth == 0);
    --depth;
    assert(depth < 200'000u);
    // если нет объектов, то ничего не делать
    return_if (list.size() < 2);

    // определить какое место занимают объекты
    Vec lu, rd;
    define_dimension(lu, rd);

    // определить по какой оси лучше поделиться
    cauto diff = rd - lu;
    x_axis = diff.x > diff.y;
    
    // определить размеры двух половинок
    Rect area_a, area_b;
    real mid;
    if (x_axis) {
      mid = diff.x / 2;
      area_a = Rect(area.pos, Vec(mid, area.size.y));
      area_b = Rect(
        Vec(area.pos.x + mid, area.pos.y),
        Vec(area.size.x - mid, area.size.y)
      );
    } else {
      mid = diff.y / 2;
      area_a = Rect(area.pos, Vec(area.size.x, mid));
      area_b = Rect(
        Vec(area.pos.x, area.pos.y + mid),
        Vec(area.size.x, area.size.y - mid)
      );
    } // set area

    // сделать две половинки и перенести туда объекты
    a = new_unique<Node>();
    b = new_unique<Node>();
    a->is_active = true;
    b->is_active = true;
    a->area = area_a;
    b->area = area_b;

    // поделить объекты по ветвям
    for (cnauto ent: list) {
      if (x_axis) {
        if (ent->phys.get_pos().x < area_a.pos.x + area_a.size.x)
          a->list.push_back(ent);
        else
          b->list.push_back(ent);
      } else {
        if (ent->phys.get_pos().y < area_a.pos.y + area_a.size.y)
          a->list.push_back(ent);
        else
          b->list.push_back(ent);
      }
    }
    list.clear();
    a->split(depth);
    b->split(depth);
  } // split

  inline void define_dimension(Vec& lu, Vec& rd) {
    // найти крайние точки слева сверху и справа снизу
    lu = { 1'000'000,  1'000'000};
    rd = {-1'000'000, -1'000'000};
    for (cnauto ent: list) {
      cauto pos = ent->phys.get_pos();
      lu.x = std::min(lu.x, pos.x);
      lu.y = std::min(lu.y, pos.y);
      rd.x = std::max(rd.x, pos.x);
      rd.y = std::max(rd.y, pos.y);
    }
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) const {
    auto rect = area;
    rect.pos += camera_offset;
    //draw_rect<&blend_diff>(dst, rect, Pal8::white);
    draw_rect(dst, rect, Pal8::white);
    if (a)
      a->debug_draw(dst, camera_offset);
    if (b)
      b->debug_draw(dst, camera_offset);
  }
}; // Node

struct Collider_2d_tree::Impl {
  Rect m_world {}; // граница действия алгоритма
  Unique<Node> m_root {};
  uint m_depth {}; // до какой глубины продолжать деление

  explicit inline Impl(uint depth)
  : m_depth {depth} {
    assert(m_depth > 0);
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
    return_if(collidables.size() < 2);

    make_tree(collidables);
    process_collisions();
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) {
    return_if (!m_root);
    m_root->debug_draw(dst, camera_offset);
  }

  // определить в каких пределах работает алгоритм
  inline void find_area(CN<Collidables> entities) {
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

    m_world.pos = lu;
    m_world.size = rd - lu;
  }

  inline void make_tree(CN<Collidables> entities) {
    find_area(entities);
    m_root = {};

    if (!m_root) {
      m_root = new_unique<Node>();
      m_root->area = m_world;
      m_root->is_active = true;
      m_root->list = entities;
      m_root->split(m_depth);
    }
  }

  inline void process_collisions() {
    
  }
}; // Impl

Collider_2d_tree::Collider_2d_tree(uint depth): impl {new_unique<Impl>(depth)} {}
Collider_2d_tree::~Collider_2d_tree() {}
void Collider_2d_tree::operator()(CN<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_2d_tree::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
