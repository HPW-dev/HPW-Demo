#include <algorithm>
#include <utility>
#include "player.hpp"
#include "util/error.hpp"
#include "game/core/entities.hpp"
#include "game/core/sounds.hpp"
#include "game/util/camera.hpp"
#include "entity/util/entity-util.hpp"

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
  update_sound();
}

void Player::draw(Image& dst, const Vec offset) const {
  Ability_entry::draw_bg(dst, offset);
  Collidable::draw(dst, offset);
  Ability_entry::draw_fg(dst, offset);
}

void Player::process_kill() {
  Collidable::process_kill();
  clear_abilities();
  // тряхнуть камерой при смерти
  graphic::camera->add_shake(999);
}

void Player::sub_en(hp_t val) {
  energy = std::max<hp_t>(0,
    energy - val * energy_consumption);
}

void Player::update_sound() {
  // движение слушателя
  assert(hpw::sound_mgr);
  auto pos = to_sound_pos(phys.get_pos());
  pos.z = -0.1f;
  hpw::sound_mgr->set_listener_pos(pos);
}
