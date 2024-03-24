#pragma once
#include <cstdint>
#include <utility>
#include <bit>
#include "robin-hood-hashing/robin_hood.h"
#include "util/mempool.hpp"
#include "util/platform.hpp"
#include "util/math/num-types.hpp"
#include "collider.hpp"

class Entity;
class Qtree;

/// Нахождение столкновений с помощью разбиения пространства Quad Tree
class Collider_qtree final: public Collider {
  nocopy(Collider_qtree);
  Pool_ptr(Qtree) root {}; /// старт дерева

  /// проверка хитбоксов на пересечения
  void test_collide_pair(Entity& a, Entity& b);
  /// фильтрует список объектов для проверки по возможности сталкиваться
  Entitys update_qtree(CN<Entitys> entities);
  /// обновляет список пар столкновений
  void update_pairs(CN<Entitys> entities);

public:
  using Collision_pair = std::pair<Entity*, Entity*>;

  /// хешер для collision_pairs
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

  /// список пар для проверок
  robin_hood::unordered_set<Collision_pair, Collision_pairs_hash> collision_pairs {};
  /** за пределами области QTree, пересечения объектов не будут находится
  * @param depth сколько раз можно делить пространство
  * @param entity_limit сколько должно быть объектов в секторе, чтобы начать деление
  * @param X ширина пространства
  * @param Y высота пространства */
  explicit Collider_qtree(uint depth, uint entity_limit, std::size_t X, std::size_t Y);
  ~Collider_qtree();
  void operator()(CN<Entitys> entities, double dt) override;
  /// рисует сетку разделений для дебага
  void debug_draw(Image& dst, const Vec camera_offset) override;
}; // Collider_qtree
