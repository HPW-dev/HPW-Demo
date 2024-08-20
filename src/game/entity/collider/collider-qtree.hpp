#pragma once
#include <bit>
#include <cstdint>
#include <utility>
#include "robin-hood-hashing/robin_hood.h"
#include "util/mempool.hpp"
#include "util/platform.hpp"
#include "util/math/num-types.hpp"
#include "collider.hpp"

class Collidable;
using Collidables = Vector<Collidable*>;

// Нахождение столкновений с помощью разбиения пространства Quad Tree
class Collider_qtree final: public Collider {
  nocopy(Collider_qtree);
  class Qtree;
  Pool_ptr(Qtree) root {}; // старт дерева

  // проверка хитбоксов на пересеченияCollidableя
  void test_collide_pair(Collidable& a, Collidable& b);
  // фильтрует список объектов для проверки по возможности сталкиваться
  Collidables update_qtree(cr<Entities> entities);
  // обновляет список пар столкновений
  void update_pairs(cr<Collidables> entities);

public:
  using Collision_pair = std::pair<Collidable*, Collidable*>;
  // память под ветви
  Mem_pool qtree_mempool;

  // хешер для collision_pairs
  struct Collision_pairs_hash {
    inline static std::size_t operator()(const Collision_pair val) {
      // сделать поинтеры числом и юзать это как уникальный ID
      cauto a = std::bit_cast<std::uintptr_t>(val.first);
      cauto b = std::bit_cast<std::uintptr_t>(val.second);
      #ifdef is_x32
        return (a << 15) + b;
      #else
        return (a << 31) + b;
      #endif
    }
  };

  // список пар для проверок
  robin_hood::unordered_set<Collision_pair, Collision_pairs_hash> collision_pairs {};
  /** за пределами области QTree, пересечения объектов не будут находится
  * @param depth сколько раз можно делить пространство
  * @param entity_limit сколько должно быть объектов в секторе, чтобы начать деление
  * @param X ширина пространства
  * @param Y высота пространства */
  explicit Collider_qtree(uint depth, uint entity_limit, std::size_t X, std::size_t Y);
  ~Collider_qtree();
  void operator()(cr<Entities> entities, Delta_time dt) override;
  // рисует сетку разделений для дебага
  void debug_draw(Image& dst, const Vec camera_offset) override;
}; // Collider_qtree
