#include <cassert>
#include <typeinfo>
#include "invise.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "util/error.hpp"

struct Ability_invise::Impl {
  nocopy(Impl);
  /* уровни способности:
  0 - невидимость игрока
  1 - невидимость всех
  2 - невидимость всех и пуль
  3 - чёрный экран */
  uint m_power {};

  inline explicit Impl(CN<Player> player) {}

  inline void update(Player& player, const double dt) {
    switch (m_power) {
      // чёрный экран
      default:
      case 3: {
        error("impl: добавь выключение игрового экрана");
        break;
      }

      // инвиз всего
      case 2: {
        hpw::entity_mgr->set_visible(false);
        break;
      }

      // инвиз врагов и игрока
      case 1: {
        nauto entities = hpw::entity_mgr->get_entities();
        for (nauto entity: entities) {
          if (entity->status.live && entity->status.is_enemy)
            entity->status.disable_render = true;
        }
      }

      // инвиз только игрока
      case 0: {
        // когда игрок стреляет, его видно
        player.status.disable_render = !is_pressed(hpw::keycode::shoot);
        break;
      }
    } // switch power
  } // update

  inline void powerup() {
    ++m_power;
    // TODO множитель очков + 0.3 * m_power
  }

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

Ability_invise::Ability_invise(CN<Player> player)
  : Ability {typeid(Ability_invise).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_invise::~Ability_invise() {}
void Ability_invise::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_invise::powerup() { impl->powerup(); }
utf32 Ability_invise::name() const { return impl->name(); }
utf32 Ability_invise::desc() const { return impl->desc(); }
