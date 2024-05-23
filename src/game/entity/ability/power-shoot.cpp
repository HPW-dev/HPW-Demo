#include <typeinfo>
#include "game/entity/player.hpp"
#include "power-shoot.hpp"
#include "game/util/game-util.hpp"

struct Ability_power_shoot::Impl {
  nocopy(Impl);
  inline explicit Impl() {}

  inline void update(Player& player, const double dt) {}
  inline void powerup() {}
  inline utf32 name() const { return get_locale_str("plyaer.ability.power_shoot.name"); }
  inline utf32 desc() const { return get_locale_str("plyaer.ability.power_shoot.desc"); }
}; // Impl

Ability_power_shoot::Ability_power_shoot()
  : Ability {typeid(Ability_power_shoot).hash_code()}
  , impl {new_unique<Impl>()} {}
Ability_power_shoot::~Ability_power_shoot() {}
void Ability_power_shoot::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_power_shoot::powerup() { impl->powerup(); }
utf32 Ability_power_shoot::name() const { return impl->name(); }
utf32 Ability_power_shoot::desc() const { return impl->desc(); }
