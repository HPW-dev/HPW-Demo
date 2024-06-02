#include <cassert>
#include <typeinfo>
#include "home.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"

struct Ability_home::Impl {
  nocopy(Impl);
  uint m_power {};

  inline explicit Impl(CN<Player> player) {}
  inline void update(Player& player, const Delta_time dt) {}
  inline void power_up() {}
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

  inline CP<Sprite> icon() const {
    return {}; // TODO
  }
}; // Impl

Ability_home::Ability_home(CN<Player> player)
  : Ability {typeid(Ability_home).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_home::~Ability_home() {}
void Ability_home::update(Player& player, const Delta_time dt) { impl->update(player, dt); }
void Ability_home::power_up() { impl->power_up(); }
utf32 Ability_home::name() const { return impl->name(); }
utf32 Ability_home::desc() const { return impl->desc(); }
CP<Sprite> Ability_home::icon() const { return impl->icon(); }
