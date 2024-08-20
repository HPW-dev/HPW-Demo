#include <algorithm>
#include <utility>
#include "player.hpp"
#include "util/error.hpp"
#include "game/core/entities.hpp"
#include "game/util/camera.hpp"

Player::Player(): Collidable(GET_SELF_TYPE), Ability_entry(*this) {
  hpw::entity_mgr->set_player(this);
  status.layer_up = true; // игрок всегда сверху
  status.ignore_bound = true; // не убивать игрока, когда он за экраном
  status.is_player = true;
}

Player::~Player() {
  if (hpw::entity_mgr)
    hpw::entity_mgr->set_player({});
}

void Player::update(const Delta_time dt) {
  Collidable::update(dt);
  Ability_entry::update(dt);
  // трясти камеру при столкновениях
  if (this->status.collided)
    graphic::camera->add_shake(999);
}

void Player::draw(Image& dst, const Vec offset) const {
  Ability_entry::draw_bg(dst, offset);
  Collidable::draw(dst, offset);
  Ability_entry::draw_fg(dst, offset);
}

void Player::kill() {
  Collidable::kill();
  Ability_entry::clear();
  // тряхнуть камерой при смерти
  graphic::camera->add_shake(999);
}

void Player::sub_en(hp_t val) { energy = std::max<hp_t>(0, energy - val); }
