#include <omp.h>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include "collider-qtree.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/core/debug.hpp"
#include "host/command.hpp"
#include "util/math/rect.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

// сколько нужно игровых объектов чтобы запустить многопоточный поиск в QTree
constexpr std::size_t MAX_ENTS_FOR_MULTY_TEST_PAIRS = 300;
// сколько нужно игровых объектов чтобы запустить многопоточный просчёт столкновений
constexpr std::size_t MAX_ENTS_FOR_MULTY_UPDATE = 75;

// quad-tree односвязное дерево
class Collider_qtree::Qtree final {
  Collidables m_entitys {}; // список объектов в текущей ноде
  Pool_ptr(Qtree) lu {}; // лево верх
  Pool_ptr(Qtree) ru {}; // право верх
  Pool_ptr(Qtree) ld {}; // лево низ
  Pool_ptr(Qtree) rd {}; // право низ
  bool have_branches {}; // если true, есть деление на ноды

  // https://www.tutorialspoint.com/circle-and-rectangle-overlapping-in-cplusplus
  static inline auto eval(auto a, auto b, auto c)
    { return std::max(b, std::min(a, c)); }

  // https://www.tutorialspoint.com/circle-and-rectangle-overlapping-in-cplusplus
  static inline bool intersect(cr<Rect> a, cr<Circle> b) {
    auto cx = b.offset.x;
    auto cy = b.offset.y;
    auto r = b.r;
    auto left = a.pos.x;
    auto right = a.pos.x + a.size.x;
    auto bottom = a.pos.y;
    auto top = a.pos.y + a.size.y;

    auto cdx = eval(cx, left, right);
    auto cdy = eval(cy, bottom, top);
    auto dx = cx - cdx;
    auto dy = cy - cdy;
    auto disSq = (dx * dx) + (dy * dy);
    auto sqrRadius = r * r;
    return (disSq <= sqrRadius);
  } // intersect

public:
  Rect bound {}; // размер ограничивающей области в текущей ноде
  std::size_t depth {}; // текущая глубина ноды
  std::size_t max_depth {}; // макс. глубина деления нод
  std::size_t entity_limit {}; // макс. число объектов в ноде
  Collider_qtree* m_master {};

  ~Qtree() = default;

  inline Qtree(cr<Rect> _bound, std::size_t _depth, std::size_t _max_depth,
  std::size_t _entity_limit)
  : bound( _bound )
  , depth( _depth )
  , max_depth( _max_depth )
  , entity_limit( _entity_limit )
  {
    assert(entity_limit > 0);
    assert(bound.size.x * bound.size.y > 0);
  }

  // добавить объект в систему
  inline void add(Collidable& entity) {
    // если есть ветви, то записываем объекы в них
    if (have_branches) {
      auto pos_x = std::floor(entity.phys.get_pos().x);
      auto pos_y = std::floor(entity.phys.get_pos().y);
      // определение, в какую ветвь кинуть объект:
      // верх
      if (pos_y >= bound.pos.y && pos_y < bound.pos.y + (bound.size.y / 2.0)) {
        if (pos_x >= bound.pos.x && pos_x < bound.pos.x + (bound.size.x / 2.0))
          lu->add(entity); // лево
        else
          ru->add(entity); // право
      } else { // низ
        if (pos_x >= bound.pos.x && pos_x < bound.pos.x + (bound.size.x / 2.0))
          ld->add(entity); // лево
        else
          rd->add(entity); // право
      }
    } else { // если ветвей нет:
      // добавить объект, если есть свободное место
      if (m_entitys.size() < entity_limit) {
        m_entitys.emplace_back(&entity);
      } else { // при отстуствии свободного места:
        // если размер вставляемого объекта больше сектора, то не делить пространство
        auto hithox = entity.get_hitbox();
        return_if( !hithox);
        if (
          std::abs(hithox->simple.offset.x) + hithox->simple.r > bound.size.x / 2.0 ||
          std::abs(hithox->simple.offset.y) + hithox->simple.r > bound.size.y / 2.0
        ) {
          m_entitys.emplace_back(&entity);
          return;
        }
        // поделиться на 4 части, если глубина меньше лимита объектов в ноде
        if (depth < max_depth) {
          split();
          add(entity); // добавить что хотели в следующие ветви
        } else { // не делиться больше, добавить объект
          m_entitys.emplace_back(&entity);
        }
      } // else m_entitys.size() >= entity_limit
    } // if not have_branches
  } // add

  // поделить ноду на 4 части
  inline void split() {
    auto rect_size = Vec(bound.size.x / 2.0, bound.size.y / 2.0);
    Rect lu_rect {Vec(bound.pos.x, bound.pos.y), rect_size};
    Rect ru_rect {Vec(bound.pos.x + rect_size.x, bound.pos.y), rect_size};
    Rect ld_rect {Vec(bound.pos.x, bound.pos.y + rect_size.y), rect_size};
    Rect rd_rect {Vec(bound.pos.x + rect_size.x, bound.pos.y + rect_size.y), rect_size};
    have_branches = true;
    lu = m_master->qtree_mempool.new_object<Qtree>(lu_rect, depth + 1, max_depth, entity_limit);
    ru = m_master->qtree_mempool.new_object<Qtree>(ru_rect, depth + 1, max_depth, entity_limit);
    ld = m_master->qtree_mempool.new_object<Qtree>(ld_rect, depth + 1, max_depth, entity_limit);
    rd = m_master->qtree_mempool.new_object<Qtree>(rd_rect, depth + 1, max_depth, entity_limit);
    lu->m_master = m_master;
    ru->m_master = m_master;
    ld->m_master = m_master;
    rd->m_master = m_master;
    // перенос объектов из ветки выше в новые
    for (rauto entity: m_entitys)
      add(*entity);
    m_entitys.clear();
  } // split

  inline void draw(Image& dst, const Vec camera_offset) const {
    return_if (!lu); // если нет веток, то не рисовать деления

    // нарисовать разделяющую сетку
    Pal8 grid_color = Pal8::white;
    draw_line<&blend_diff>(dst, 
      Vec(camera_offset.x + bound.pos.x + (bound.size.x / 2.0),
        std::floor(bound.pos.y)),
      Vec(camera_offset.x + bound.pos.x + (bound.size.x / 2.0),
        std::ceil(bound.pos.y + bound.size.y)),
      grid_color);
    draw_line<&blend_diff>(dst, 
      Vec(std::floor(bound.pos.x),
        camera_offset.y + bound.pos.y + (bound.size.y / 2.0)),
      Vec(std::ceil(bound.pos.x + bound.size.x),
        camera_offset.y + bound.pos.y + (bound.size.y / 2.0)),
      grid_color);
    if (have_branches) {
      lu->draw(dst, camera_offset);
      ru->draw(dst, camera_offset);
      ld->draw(dst, camera_offset);
      rd->draw(dst, camera_offset);
    }
  } // draw

  // найти соседей в области area
  inline void find(cr<Circle> area, Collidables& list) const {
    if (intersect(this->bound, area)) {
      // так быстрее, чем std::copy или list.insert
      for (crauto en: m_entitys)
        list.emplace_back(en);

      if (have_branches) {
        lu->find(area, list);
        ru->find(area, list);
        ld->find(area, list);
        rd->find(area, list);
      }
    } // if intersected
  } // find

  // подсчитывает число активных нод
  inline void node_count(uint& cnt) const {
    ++cnt;
    if (have_branches) {
      lu->node_count(cnt);
      ru->node_count(cnt);
      ld->node_count(cnt);
      rd->node_count(cnt);
    }
  }

  inline void clear() {
    m_entitys.clear();

    if (have_branches) {
      lu->clear();
      ld->clear();
      ru->clear();
      rd->clear();
    }
  }
}; // Qtree

Collider_qtree::~Collider_qtree() {}

Collider_qtree::Collider_qtree(uint max_depth, uint entity_limit,
std::size_t X, std::size_t Y) {
  root = qtree_mempool.new_object<Qtree>(
    Rect(0, 0, std::max(X, Y), std::max(X, Y)),
    0, max_depth, entity_limit);
  root->m_master = this;
}

void Collider_qtree::operator()(cr<Entities> entities, Delta_time dt) {
  auto filtered_entitys = update_qtree(entities);
  update_pairs(filtered_entitys);

  if (collision_pairs.size() >= MAX_ENTS_FOR_MULTY_TEST_PAIRS) {
    // перегонка unordered_set в vector, чтобы через omp можно было распараллелить:
    Vector<Collision_pair> tmp_pairs(collision_pairs.begin(), collision_pairs.end());
    #pragma omp parallel for schedule(dynamic, 4)
    cfor (i, tmp_pairs.size()) {
      crauto pair = tmp_pairs[i];
      test_collide_pair(*pair.first, *pair.second);
    }
  } else {
    for (crauto pair: collision_pairs)
      test_collide_pair(*pair.first, *pair.second);
  }
}

void Collider_qtree::test_collide_pair(Collidable& a, Collidable& b) {
  // объектыs что сюда попадут, точно можно будет сталкивать
  return_if (a.is_collided_with(&b));
  if (a.hitbox_test(b))
    a.collide_with(b);
}

void Collider_qtree::debug_draw(Image& dst, const Vec camera_offset) {
  root->draw(dst, camera_offset);
}

Collidables Collider_qtree::update_qtree(cr<Entities> entities) {
  Collidables ret; // объекты пригодные к сталкиванию

  #ifdef ECOMEM
    // сбросить листы в дереве
    auto bound = root->bound;
    auto max_depth = root->max_depth;
    auto entity_limit = root->entity_limit;
    qtree_mempool.release();
    root = qtree_mempool.new_object<Qtree>(bound, 0, max_depth, entity_limit);
  #else
    // не удалять память ветвей, просто очистить списки элементов в ветвях
    root->clear();
  #endif
  
  // перестроить всё дерево заново
  for (rauto entity_p: entities) {
    assert(entity_p);
    // проверить что объект может сталкиваться и что он жив
    if (entity_p->status.live && entity_p->status.collidable) {
      auto entity = ptr2ptr<Collidable*>(entity_p.get());
      root->add(*entity);
      ret.emplace_back(entity);
    }
  }

  return ret;
} // update_qtree

void Collider_qtree::update_pairs(cr<Collidables> entities) {
  collision_pairs.clear();

  if (entities.size() >= MAX_ENTS_FOR_MULTY_UPDATE) {
    // узнать сколько потоков сделал OpenMP
    #ifdef _OPENMP
      auto th_max = omp_get_max_threads();
    #else
      constexpr const auto th_max = 1;
    #endif
    assert(th_max > 0);
    // для хранения локальных списков в потоках
    static Vector<decltype(collision_pairs)> list_table;
    static Vector<Collidables> lists;
    list_table.resize(th_max);
    for (rauto table: list_table)
      table.clear();
    lists.resize(th_max);

    /* проверить области вокруг каждой точки и закинуть в пару
    коллизии соседние ноды входящие в область */
    #pragma omp parallel for \
      schedule(dynamic) \
      shared(root, list_table)
    for (rauto entity: entities) {
      auto hitbox = entity->get_hitbox();
      cont_if(!hitbox);
      // узнать какой сейчас поток
      #ifdef _OPENMP
        cauto th_idx = omp_get_thread_num();
      #else
        constexpr const auto th_idx = 0;
      #endif
      lists[th_idx].clear();
      // искать соседей вокруг хитбокса объекта
      cauto pos = entity->phys.get_pos();
      const Circle area(pos + hitbox->simple.offset, hitbox->simple.r * 2);
      // найти соседей к этой точке
      root->find(area, lists[th_idx]);

      // добавить этих соседей в пары на проверки
      for (rauto other: lists[th_idx]) {
        auto addr_a = entity;
        auto addr_b = other;
        cont_if (!addr_a->collision_possible(*addr_b));
        if (addr_b > addr_a)
          std::swap(addr_a, addr_b);
        // unordered_set сам уберёт повторы
        list_table.at(th_idx).emplace(addr_a, addr_b);
      } // for list
    } // for entities

    // объединение листов с потоков в релизный collision_pairs
    for (crauto table: list_table) {
      for (rauto it: table) {
        auto addr_a = it.first;
        auto addr_b = it.second;
        cont_if (!addr_a->collision_possible(*addr_b));
        if (addr_b > addr_a)
          std::swap(addr_a, addr_b);
        // unordered_set сам уберёт повторы
        collision_pairs.emplace( std::move(it) );
      } // for table
    } // for list_table
  } else { // вариант без многопотока
    /* проверить области вокруг каждой точки и закинуть в пару
    коллизии соседние ноды входящие в область */
    for (rauto entity: entities) {
      auto hitbox = entity->get_hitbox();
      cont_if(!hitbox);
      auto pos = entity->phys.get_pos();

      // искать соседей вокруг хитбокса объекта
      Circle area(pos + hitbox->simple.offset, hitbox->simple.r * 2);
      
      // найти соседей к этой точке
      Collidables list;
      root->find(area, list);

      // добавить этих соседей в пары на проверки
      for (rauto other: list) {
        auto addr_a = entity;
        auto addr_b = other;
        cont_if (!addr_a->collision_possible(*addr_b));
        // эта перестановка сократит число одинаковых пар
        if (addr_b > addr_a)
          std::swap(addr_a, addr_b);
        // unordered_set сам уберёт повторы
        collision_pairs.emplace(addr_a, addr_b);
      } // for list
    } // for entities
  }
} // update_pairs
