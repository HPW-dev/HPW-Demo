#include <cassert>
#include <typeinfo>
#include "invise.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"

struct Ability_invise::Impl {
  nocopy(Impl);
  uint m_power {};

  inline explicit Impl() {}
  inline void update(Player& player, const double dt) {}
  inline void powerup() {}
  inline utf32 name() const { return get_locale_str("plyaer.ability.invise.name"); }
  inline utf32 desc() const {
    switch (m_power) {
      case 0: return get_locale_str("plyaer.ability.invise.desc_0"); break;
      case 1: return get_locale_str("plyaer.ability.invise.desc_1"); break;
      case 2: return get_locale_str("plyaer.ability.invise.desc_2"); break;
      default:
      case 3: return get_locale_str("plyaer.ability.invise.desc_3"); break;
    }
    return {};
  }
}; // Impl

Ability_invise::Ability_invise()
  : Ability {typeid(Ability_invise).hash_code()}
  , impl {new_unique<Impl>()} {}
Ability_invise::~Ability_invise() {}
void Ability_invise::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_invise::powerup() { impl->powerup(); }
utf32 Ability_invise::name() const { return impl->name(); }
utf32 Ability_invise::desc() const { return impl->desc(); }
