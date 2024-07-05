#include <algorithm>
#include "player.hpp"
#include "util/error.hpp"
#include "game/entity/player/ability/ability.hpp"
#include "game/core/entities.hpp"
#include "game/util/camera.hpp"

Player::Player(): Collidable(GET_SELF_TYPE) {
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
  // трясти камеру при столкновениях
  if (this->status.collided)
    graphic::camera->add_shake(999);
  // применить способности
  for (cnauto it: capabilities)
    it->update(*this, dt);
}

Ability* Player::move_ability(Shared<Ability>&& ability) {
  // если такая способность уже есть у игрока, то апнуть её
  for (cnauto it: capabilities)
    if (it->type_id() == ability->type_id()) {
      it->power_up();
      return it.get();
    }
  // если способности нет у игрока, то добавить её в список
  auto ret = capabilities.emplace_back( std::move(ability) );
  return ret.get();
}

Ability* Player::find_ability(const std::size_t type_id) const {
  cnauto it = std::find_if (
    capabilities.begin(), capabilities.end(),
    [type_id](CN<decltype(capabilities)::value_type> ability)
      { return ability->type_id() == type_id; }
  );
  if (it != capabilities.end())
    it->get();
  return {};
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

void Player::sub_en(hp_t val) { energy = std::max<hp_t>(0, energy - val); }
void Player::remove_abilities() { capabilities.clear(); }
