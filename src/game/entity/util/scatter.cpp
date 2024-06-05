#include <cassert>
#include "scatter.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/util/phys.hpp"
#include "game/util/camera.hpp"
#include "game/core/entities.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/math/random.hpp"
#include "util/hpw-util.hpp"

void Scatter::accept(Entity& dst) const {
  return_if( !hpw::entity_mgr->get_player());
  return_if(range == 0);
  return_if(power == 0);

  auto len = distance(this->pos, dst.phys.get_pos());
  return_if (len > this->range); // за пределами действия объекты не трогать

  real intense = 1.0 - (len / range);

  switch (type) {
    default:
    case Type::outside: {
      auto direct = intense ? normalize_stable(dst.phys.get_pos() - this->pos) : rand_normalized_stable();
      dst.phys.set_vel( dst.phys.get_vel() + direct * this->power * intense );
      break;
    }
    case Type::inside: {
      auto direct = intense ? normalize_stable(dst.phys.get_pos() - this->pos) : rand_normalized_stable();
      dst.phys.set_vel( dst.phys.get_vel() - direct * this->power * intense );
      break;
    }
    case Type::random: {
      auto direct = rand_normalized_stable();
      dst.phys.set_vel( dst.phys.get_vel() + direct * this->power * intense );
      break;
    }
  } // switch type

  // добавить тряску игроку, если он рядом
  cauto player_p = cptr2ptr<CP<Entity>>(hpw::entity_mgr->get_player());
  assert(player_p);
  if (!disable_shake && player_p->status.live && std::addressof(dst) == player_p)
    graphic::camera->add_shake(intense);
}
