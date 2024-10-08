#include "entity-loader.hpp"
#include "entity.hpp"
#include "util/entity-util.hpp"
#include "util/math/vec.hpp"

void Entity_loader::prepare(Entity& dst, Entity* master, const Vec pos) {
  dst.set_master(master);
  dst.set_pos(pos);
  // передача импульса от master
  if (master) {
    dst.phys.set_deg(master->phys.get_deg());
    dst.phys.set_speed(master->phys.get_speed());
  }
  dst.status.live = true;
}
