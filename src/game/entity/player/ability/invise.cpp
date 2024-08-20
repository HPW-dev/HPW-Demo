#include <cassert>
#include <typeinfo>
#include "invise.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/util/score-table.hpp"

struct Ability_invise::Impl {
  nocopy(Impl);
  /* уровни способности:
  0 - невидимость игрока
  1 - невидимость всех
  2 - невидимость всех и пуль
  3 - чёрный экран */
  uint m_power {};

  inline explicit Impl(cr<Player> player) {}

  inline void update(Player& player, const Delta_time dt) {
    switch (m_power) {
      // чёрный экран
      default:
      case 3: {
        hpw::level_mgr->set_visible(false);
        // break тут не нужен
      }

      // инвиз всего
      case 2: {
        rauto entities = hpw::entity_mgr->get_entities();
        for (rauto entity: entities) {
          if (
            // нужны живые объекты
            entity->status.live &&
            // учитываются только игрок, противники и пули
            (
              entity->status.is_enemy ||
              entity->status.is_player ||
              entity->status.is_bullet
            )
          ) {
            entity->status.disable_render = true;
            entity->status.no_motion_interp = true;
          }
        }
        break;
      }

      // инвиз врагов и игрока
      case 1: {
        rauto entities = hpw::entity_mgr->get_entities();
        for (rauto entity: entities) {
          if (entity->status.live && entity->status.is_enemy) {
            if (entity->status.collided) {
              entity->move_update_callback( Timed_visible(0.25) );
            } else { // если с врагом столкнулись, то подсветить его
              entity->status.disable_render = true;
              entity->status.no_motion_interp = true;
            }
          }
        }
        // break тут не нужен
      } // lvl 1

      // инвиз только игрока
      case 0: {
        // когда игрок стреляет, его видно
        player.status.disable_render = !is_pressed(hpw::keycode::shoot);
        player.status.no_motion_interp = true;
        break;
      }
    } // switch power
  } // update

  inline void power_up() {
    ++m_power;
    cauto scale = hpw::get_score_scale();
    hpw::set_score_scale(scale + 0.3 * m_power);
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

  inline cp<Sprite> icon() const {
    return {}; // TODO
  }
}; // Impl

Ability_invise::Ability_invise(cr<Player> player)
  : Ability {typeid(Ability_invise).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_invise::~Ability_invise() {}
void Ability_invise::update(Player& player, const Delta_time dt) { impl->update(player, dt); }
void Ability_invise::power_up() { impl->power_up(); }
utf32 Ability_invise::name() const { return impl->name(); }
utf32 Ability_invise::desc() const { return impl->desc(); }
cp<Sprite> Ability_invise::icon() const { return impl->icon(); }
