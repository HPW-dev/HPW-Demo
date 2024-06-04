#include <cassert>
#include <typeinfo>
#include "fullscreen-shoot.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"
#include "game/core/entities.hpp"
#include "util/math/timer.hpp"
#include "util/math/random.hpp"

struct Ability_fullscreen_shoot::Impl {
  nocopy(Impl);
  Timer m_clone_timer {1.0}; // по таймеру все пули клонируются
  real m_clone_chance {0.5}; // вероятность, с которой будет создан клон пули
  uint m_power {};

  inline explicit Impl(CN<Player> player) {}

  inline void update(Player& player, const Delta_time dt) {
    return_if (!m_clone_timer.update(dt));

    // попытаться склонировать все пули в игре
    //for ()
  }

  inline void power_up() { ++m_power; }

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

  inline CP<Sprite> icon() const {
    return {}; // TODO
  }
}; // Impl

Ability_fullscreen_shoot::Ability_fullscreen_shoot(CN<Player> player)
  : Ability {typeid(Ability_fullscreen_shoot).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_fullscreen_shoot::~Ability_fullscreen_shoot() {}
void Ability_fullscreen_shoot::update(Player& player, const Delta_time dt) { impl->update(player, dt); }
void Ability_fullscreen_shoot::power_up() { impl->power_up(); }
utf32 Ability_fullscreen_shoot::name() const { return impl->name(); }
utf32 Ability_fullscreen_shoot::desc() const { return impl->desc(); }
CP<Sprite> Ability_fullscreen_shoot::icon() const { return impl->icon(); }
