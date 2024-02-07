#include "entity-loader.hpp"
#include "entity.hpp"
#include "util/entity-util.hpp"
#include "util/math/vec.hpp"

void Entity_loader::prepare(Entity& dst, Entity* master, const Vec pos) {
  // TODO передача импульса от master
  dst.set_master(master);
  dst.set_pos(pos);
  if (master) {  
    dst.phys.set_vel(master->phys.get_vel());
  }
  dst.status.live = true;
}
