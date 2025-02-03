#include <cassert>
#include <utility>
#include "entity.hpp"
#include "util/error.hpp"
#include "game/core/core.hpp"
#include "game/entity/entity-mgr.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/core/canvas.hpp"

Entity::Entity()
: Entity_animated (*this)
, uid (get_entity_uid())
, type {GET_SELF_TYPE}
{
  status.live = true;
}

Entity::Entity(Entity_type new_type): Entity() { type = new_type; }

void Entity::kill() {
  status.live = false;
  status.killme = true;
  remove();
}

void Entity::remove() {
  status.live = false;
  status.removeme = true;
}

void Entity::process_kill() {
  status.killme = false;
  process_kill_cbs();
}

void Entity::process_remove() {
  status.removed = true;
  process_remove_cbs();
}

void Entity::update(const Delta_time dt) {
  _old_pos = _cur_pos;
  Entity_animated::update(dt);

  if (!status.disable_motion)
    move_it(dt);

  process_update_cbs(dt);
  check_out_of_screen();
  _cur_pos = phys.get_pos();
}

void Entity::set_master(Master new_master) {
  assert(new_master != this);
  _master = new_master;
}

void Entity::check_out_of_screen() {
  cauto pos = phys.get_pos();
  constexpr real BOUND = 50;
  status.out_of_screen =
    (pos.x <= -BOUND) ||
    (pos.x >= graphic::width + BOUND) ||
    (pos.y <= -BOUND) ||
    (pos.y >= graphic::height + BOUND);
}

void Entity::move_it(const Delta_time dt) { phys.update(dt); }
void Entity::set_pos(const Vec pos) { phys.set_pos(pos); }
