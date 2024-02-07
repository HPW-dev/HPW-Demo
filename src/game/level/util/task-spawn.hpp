#pragma once
#include <utility>
#include <functional>
#include "util/vector-types.hpp"
#include "util/math/timer.hpp"

class Entity;

/* Задача на спавн противников. Продолжение по истечению
таймера или смерти всех противников */
class Spwan_and_wait_for_death final {
public:
  using Death_list = Vector<Entity*>;
  using spawn_pf = std::function<Death_list ()>;

  explicit Spwan_and_wait_for_death(spawn_pf&& spawn_f, double timeout);
  bool operator()(double dt);

private:
  bool m_once_call {true};
  spawn_pf m_spawn_f;
  Timer m_timeout;
  Death_list m_death_list;
}; // Spwan_and_wait_for_death
