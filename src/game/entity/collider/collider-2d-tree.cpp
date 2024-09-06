#include <omp.h>
#include <cassert>
#include <algorithm>
#include "collider-2d-tree.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/mem-types.hpp"

using Collidables = Vector<Collidable*>;

inline void test_collide(Collidable& a, Collidable& b) {
  return_if (!a.collision_possible(b));
  return_if (!a.hitbox_test(b));
  a.collide_with(b);
}

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

    Rect area_a, area_b;
    return_if(!find_axis_and_areas(lu, rd, area_a, area_b));

    // сделать две половинки и перенести туда объекты
    if (!a)
      init_unique(a);
    if (!b)
      init_unique(b);
    a->is_active = true;
    b->is_active = true;
    a->area = area_a;
    b->area = area_b;

    // поделить объекты по ветвям
    for (cauto ent: list)
      push_to_list(lu, rd, a, b, ent);

    list.clear();
    a->split(depth);
    b->split(depth);
  } // split

  // добавляет объект в нужные ветви в зависимости от выбранной оси x_axis
  inline void push_to_list(Vec lu, Vec rd,
  cr<Unique<Node>> a, cr<Unique<Node>> b, Collidable* const ent) {
    cauto pos = ent->phys.get_pos();
    crauto hitbox = ent->get_hitbox();
    assert(hitbox);

    // учесть размеры хитбокса
    lu.x = pos.x + hitbox->simple.offset.x - hitbox->simple.r;
    lu.y = pos.y + hitbox->simple.offset.y - hitbox->simple.r;
    rd.x = pos.x + hitbox->simple.offset.x + hitbox->simple.r;
    rd.y = pos.y + hitbox->simple.offset.y + hitbox->simple.r;

    if (this->x_axis) {
      if (rd.x < a->area.pos.x + a->area.size.x) {
        a->list.push_back(ent);
      } else {
        b->list.push_back(ent);
        /* если объект залазит на чужую
        территорию, то добавить его в списки там */
        if (lu.x < a->area.pos.x + a->area.size.x)
          a->list.push_back(ent);
      }
    } else {
      if (rd.y < a->area.pos.y + a->area.size.y) {
        a->list.push_back(ent);
      } else {
        b->list.push_back(ent);
        /* если объект залазит на чужую
        территорию, то добавить его в списки там */
        if (lu.y < a->area.pos.y + a->area.size.y)
          a->list.push_back(ent);
      }
    }
  }

  /* определит по какой оси лучше поделиться (изменит x_axis)
  и вычислит размеы двух половинок. Вернёт false, если делить не нужно */
  inline bool find_axis_and_areas(const Vec lu, const Vec rd, Rect& area_a, Rect& area_b) {
    // определить по какой оси лучше поделиться
    cauto diff = rd - lu;
    this->x_axis = diff.x > diff.y;
    
    // определить размеры двух половинок
    real mid;
    if (this->x_axis) {
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

    // проверить что половинки не слишком маленькие
    constexpr real AREA_THRESHOLD = 10;
    return_if(area_a.size.x <= AREA_THRESHOLD, false);
    return_if(area_a.size.y <= AREA_THRESHOLD, false);
    return_if(area_b.size.x <= AREA_THRESHOLD, false);
    return_if(area_b.size.y <= AREA_THRESHOLD, false);
    return true;
  }

  inline void define_dimension(Vec& lu, Vec& rd) {
    // найти крайние точки слева сверху и справа снизу
    lu = { 1'000'000,  1'000'000};
    rd = {-1'000'000, -1'000'000};
    for (crauto ent: list) {
      cauto pos = ent->phys.get_pos();
      lu.x = std::min(lu.x, pos.x);
      lu.y = std::min(lu.y, pos.y);
      rd.x = std::max(rd.x, pos.x);
      rd.y = std::max(rd.y, pos.y);
    }
  }

  inline void debug_draw(Image& dst, const Vec camera_offset) const {
    return_if(!is_active);
    auto rect = area;
    rect.pos += camera_offset;
    draw_rect(dst, rect, Pal8::white);
    if (a) a->debug_draw(dst, camera_offset);
    if (b) b->debug_draw(dst, camera_offset);
  }

  inline void process_collisions() {
    return_if(!is_active);

    #pragma omp task
    impl_process_collisions();
    if (a) a->process_collisions();
    if (b) b->process_collisions();
  }

  inline void impl_process_collisions() {
    cauto entitys_sz = list.size();
    return_if (entitys_sz < 2);

    if (entitys_sz == 2)
      test_collide(*list[0], *list[1]);
    
    for (std::size_t a_i = 0;       a_i < entitys_sz - 1; ++a_i)
    for (std::size_t b_i = a_i + 1; b_i < entitys_sz;     ++b_i)
      test_collide(*list[a_i], *list[b_i]);
  }

  inline void clear() {
    is_active = false;
    list.clear();
    if (a) a->clear();
    if (b) b->clear();
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

  // получить список объектов доступных для столкновения
  inline static Collidables collidable_filter(cr<Entities> entities) {
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
 
  inline void operator()(cr<Entities> entities, Delta_time dt) {
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
  inline void find_area(cr<Collidables> entities) {
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

    m_world.pos = lu;
    m_world.size = rd - lu;
  }

  inline void make_tree(cr<Collidables> entities) {
    find_area(entities);
    if (!m_root)
      init_unique(m_root);

    m_root->clear();
    m_root->is_active = true;

    m_root->area = m_world;
    m_root->list = entities;
    m_root->split( get_depth(entities) );
  }

  inline void process_collisions() {
    #pragma omp parallel
    #pragma omp single
    m_root->process_collisions();
  }

  // определяет глубину построения дерева по количеству объектов
  inline uint get_depth(cr<Collidables> entities) const {
    cauto entities_sz = entities.size();
    assert(entities_sz > 1);

    const uint depth = std::floor(std::sqrt(entities_sz));
    assert(depth > 0);
    
    return std::min(m_depth, depth);
  }
}; // Impl

Collider_2d_tree::Collider_2d_tree(uint depth): impl {new_unique<Impl>(depth)} {}
Collider_2d_tree::~Collider_2d_tree() {}
void Collider_2d_tree::operator()(cr<Entities> entities, Delta_time dt) { impl->operator()(entities, dt); }
void Collider_2d_tree::debug_draw(Image& dst, const Vec camera_offset) { impl->debug_draw(dst, camera_offset); }
