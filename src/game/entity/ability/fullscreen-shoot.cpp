#include <cassert>
#include <typeinfo>
#include "fullscreen-shoot.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"

struct Ability_fullscreen_shoot::Impl {
  nocopy(Impl);
  uint m_power {};

  inline explicit Impl(CN<Player> player) {}
  inline void update(Player& player, const double dt) {}
  inline void power_up() {}
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

Ability_fullscreen_shoot::Ability_fullscreen_shoot(CN<Player> player)
  : Ability {typeid(Ability_fullscreen_shoot).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_fullscreen_shoot::~Ability_fullscreen_shoot() {}
void Ability_fullscreen_shoot::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_fullscreen_shoot::power_up() { impl->power_up(); }
utf32 Ability_fullscreen_shoot::name() const { return impl->name(); }
utf32 Ability_fullscreen_shoot::desc() const { return impl->desc(); }
