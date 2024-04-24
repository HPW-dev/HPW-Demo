#include "player.hpp"
#include "util/error.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/core/entities.hpp"
#include "game/util/camera.hpp"

Player::Player(): Collidable(GET_SELF_TYPE) {
  hpw::entity_mgr->set_player(this);
  status.layer_up = true; // игрок всегда сверху
  status.ignore_bound = true; // не убивать игрока, когда он за экраном
}

Player::~Player() {
  if (hpw::entity_mgr)
    hpw::entity_mgr->set_player({});
}

bool Player::check_capability(const Ability ability) const {
  cauto it = std::find(capabilities.begin(), capabilities.end(), ability);
  return it != capabilities.end();
}

void Player::update(double dt) {
  Collidable::update(dt);
  // трясти камеру при столкновениях
  if (this->status.collided)
    graphic::camera->add_shake(999);
}
