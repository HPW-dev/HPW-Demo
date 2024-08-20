#include <cassert>
#include <typeinfo>
#include "speedup.hpp"
#include "game/entity/player/player.hpp"
#include "game/util/game-util.hpp"

struct Ability_speedup::Impl {
  nocopy(Impl);
  uint m_power {};

  inline explicit Impl(cr<Player> player) {}
  inline void update(Player& player, const Delta_time dt) {}
  inline void power_up() {}
  inline utf32 name() const { return get_locale_str("plyaer.ability.speedup.name"); }
  inline utf32 desc() const {
    switch (m_power) {
      case 0: return get_locale_str("plyaer.ability.speedup.desc_0"); break;
      default:
      case 1: return get_locale_str("plyaer.ability.speedup.desc_1"); break;
    }
    return {};
  }

  inline cp<Sprite> icon() const {
    return {}; // TODO
  }
}; // Impl

Ability_speedup::Ability_speedup(cr<Player> player)
  : Ability {typeid(Ability_speedup).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_speedup::~Ability_speedup() {}
void Ability_speedup::update(Player& player, const Delta_time dt) { impl->update(player, dt); }
void Ability_speedup::power_up() { impl->power_up(); }
utf32 Ability_speedup::name() const { return impl->name(); }
utf32 Ability_speedup::desc() const { return impl->desc(); }
cp<Sprite> Ability_speedup::icon() const { return impl->icon(); }
