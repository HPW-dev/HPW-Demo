#include "player.hpp"
#include "util/error.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/core/entitys.hpp"

Player::Player(): Collidable(GET_SELF_TYPE) {
  hpw::entity_mgr->set_player(this);
  status.layer_up = true; // игрок всегда сверху
  status.ignore_bound = true; // не убивать игрока, когда он за экраном
}

Player::~Player() {
  if (hpw::entity_mgr)
    hpw::entity_mgr->set_player({});
}
