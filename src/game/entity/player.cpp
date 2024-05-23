#include "player.hpp"
#include "util/error.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/ability/ability.hpp"
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

void Player::update(double dt) {
  Collidable::update(dt);
  // трясти камеру при столкновениях
  if (this->status.collided)
    graphic::camera->add_shake(999);
  // применить способности
  for (cnauto it: capabilities)
    it->update(*this, dt);
}

void Player::move_ability(Shared<Ability>&& ability) {
  // если такая способность уже есть у игрока, то апнуть её
  for (cnauto it: capabilities)
    if (it->type_id() == ability->type_id()) {
      it->powerup();
      return;
    }
  // если способности нет у игрока, то добавить её в список
  capabilities.emplace_back( std::move(ability) );
}

void Player::draw(Image& dst, const Vec offset) const {
  Collidable::draw(dst, offset);
  for (cnauto it: capabilities)
    it->draw(dst, offset);
}

void Player::kill() {
  Collidable::kill();
  // тряхнуть камерой при смерти
  graphic::camera->add_shake(999);
}
