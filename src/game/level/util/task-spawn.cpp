#include <cassert>
#include "task-spawn.hpp"
#include "util/macro.hpp"
#include "game/entity/entity.hpp"

Spwan_and_wait_for_death::Spwan_and_wait_for_death(spawn_pf&& spawn_f, double timeout)
: m_spawn_f {std::move(spawn_f)}
, m_timeout(timeout)
{
  assert(m_spawn_f);
  assert(timeout > 0);
}

bool Spwan_and_wait_for_death::operator()(double dt) {
  // при первом вызове наспавнить противников и получить на них ссылки
  if (m_once_call) {
    m_death_list = m_spawn_f();
    m_once_call = false;
  }

  // если таймер оттикал, то засчитать завершение
  if (m_timeout.update(dt))
    return true;
  // если хоть один противник жив, то это не конец
  for (cauto entity: m_death_list)
    if (entity->status.live)
      return false;
  return true;
}
