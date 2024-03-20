#pragma once
#include <cstdint>
#include <utility>
#include <bit>
#include "robin-hood-hashing/robin_hood.h"
#include "util/mempool.hpp"
#include "util/math/num-types.hpp"
#include "collider.hpp"

class Entity;
class Qtree;

/// проверка столкновения в quad tree
class Collider_qtree final: public Collider {
  nocopy(Collider_qtree);
  Pool_ptr(Qtree) root {};

  void test_collide_pair(Entity& a, Entity& b);
  /// вернёт отфильтрованный список объектов пригодный для дальнейшей проверки
  Entitys update_qtree(CN<Entitys> entities);
  void update_pairs(CN<Entitys> entities);

public:
  using Collision_pair = std::pair<Entity*, Entity*>;

  /// хешер для collision_pairs
  struct Collision_pairs_hash {
    inline std::size_t operator()(CN<Collision_pair> val) const {
      auto a = std::bit_cast<std::uintptr_t>(val.first);
      auto b = std::bit_cast<std::uintptr_t>(val.second);
      return (a << 18) + b;
    }
  };

  /// список пар для проверок
  robin_hood::unordered_set<Collision_pair, Collision_pairs_hash> collision_pairs {};

  explicit Collider_qtree(uint depth, uint entity_limit, std::size_t X, std::size_t Y);
  ~Collider_qtree();
  void operator()(CN<Entitys> entities, double dt) override;
  void debug_draw(Image& dst, const Vec camera_offset) override;
}; // Collider_qtree
