#include <cassert>
#include <typeinfo>
#include "home.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"

struct Ability_home::Impl {
  nocopy(Impl);
  uint m_power {};

  inline explicit Impl() {}
  inline void update(Player& player, const double dt) {}
  inline void powerup() {}
  inline utf32 name() const { return get_locale_str("plyaer.ability.home.name"); }
  inline utf32 desc() const {
    switch (m_power) {
      case 0: return get_locale_str("plyaer.ability.home.desc_0"); break;
      case 1: return get_locale_str("plyaer.ability.home.desc_1"); break;
      default:
      case 2: return get_locale_str("plyaer.ability.home.desc_2"); break;
    }
    return {};
  }
}; // Impl

Ability_home::Ability_home()
  : Ability {typeid(Ability_home).hash_code()}
  , impl {new_unique<Impl>()} {}
Ability_home::~Ability_home() {}
void Ability_home::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_home::powerup() { impl->powerup(); }
utf32 Ability_home::name() const { return impl->name(); }
utf32 Ability_home::desc() const { return impl->desc(); }
