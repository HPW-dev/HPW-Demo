#include <cassert>
#include <typeinfo>
#include "home.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player/player.hpp"
#include "game/util/game-util.hpp"
#include "util/error.hpp"
#include "util/math/timer.hpp"

struct Ability_home::Impl {
  nocopy(Impl);
  Entity* m_area {}; // прокси-объект означающий область похищения
  Entity* m_ally {}; // захваченный союзник
  Entity* m_possible_ally {}; // кого сейчас пытаемся захватить
  Timer m_aduction_delay {1.5}; // время на похищение
  /* Улучшения:
  0 - похищение с задержкой
  1 - мгновенное похищение, бумер удлинняется, движение ускоряется
  2 - бумер становится палкой, никого нельзя похитить
  3 - игрок становится длинно в экран */
  uint m_power {};

  inline explicit Impl(CN<Player> player) {
    //hpw::entity_mgr->make("")
  }

  inline void update(Player& player, const Delta_time dt) {
    // TODO
  }

  inline void draw(Image& dst, const Vec offset) const {
    // TODO
  }

  inline void power_up() {
    ++m_power;
    error("need impl for other levels");
    // TODO либо станем уже и длиннее, либо больше, шире и медленее
  }

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
void Ability_home::draw(Image& dst, const Vec offset) const { impl->draw(dst, offset); }
