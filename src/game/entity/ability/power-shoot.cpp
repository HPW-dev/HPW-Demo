#include <cassert>
#include <typeinfo>
#include "power-shoot.hpp"
#include "game/core/entities.hpp"
#include "game/core/core.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/util/game-util.hpp"
#include "util/math/random.hpp"
#include "util/hpw-util.hpp"

struct Ability_power_shoot::Impl {
  nocopy(Impl);
  real m_percent_power_shoot {};
  real m_percent_power_shoot_price {};
  hp_t m_power_shoot_price {}; // цена мощного выстрела
  hp_t m_energy_for_power_shoot {}; // сколько должно быть энергии для мощного выстрела

  inline explicit Impl() {
    assert(m_power_shoot_price > 0);
    assert(m_energy_for_power_shoot > 0);
    /*
    m_percent_for_power_shoot = shoot_node.get_real("percent_for_power_shoot");
    m_percent_for_power_shoot_price = shoot_node.get_real("percent_for_power_shoot_price");

    assert(m_percent_for_power_shoot > 0 && m_percent_for_power_shoot <= 100);
    assert(m_percent_for_power_shoot_price > 0 && m_percent_for_power_shoot_price <= 100);

    it.m_energy_for_power_shoot = it.energy_max * (m_percent_power_shoot / 100.0);
    it.m_power_shoot_price = it.energy_max * (m_percent_power_shoot_price / 100.0);
    */
  }

  inline void update(Player& player, const double dt) {
    return_if (player.energy < m_energy_for_power_shoot);

    player.sub_en(m_power_shoot_price);
    cfor (_, 30) { // TODO конфиг
      cauto spawn_pos = player.phys.get_pos() + Vec(rndr(-7, 7), 0); // TODO конфиг
      auto bullet = hpw::entity_mgr->make(&player, "bullet.player.mid", spawn_pos);
      // пуля смотрит вверх в шмап моде
      bullet->phys.set_deg(270);
      // передача импульса для шмап мода
      bullet->phys.set_speed( pps(rndr(13, 17)) ); // TODO конфиг
      bullet->phys.set_vel(bullet->phys.get_vel() + player.phys.get_vel());
      // разброс
      bullet->phys.set_deg(bullet->phys.get_deg() + rndr(-75, 75)); // TODO конфиг
      bullet->move_update_callback(get_spawner_small_bullets());
      bullet->status.layer_up = true;
    }

    // отдача
    hpw::entity_mgr->add_scatter(Scatter {
      .pos{ player.phys.get_pos() + Vec(0, -10) }, // создаёт источник взрыва перед ноосом
      .range{ 50 }, // TODO конфиг
      .power{ pps(60) }, // TODO конфиг
    });
  } // update

  // спавнит мелкие пульки в след за мощным выстрелом
  inline Entity::Update_callback get_spawner_small_bullets() const {
    return [this](Entity& master, double dt)->void {
      // TODO all vals by config
      // TODO by dt timer
      if ((hpw::game_updates_safe % 5) == 0) { // TODO game_updates_safe убрать
        auto it = hpw::entity_mgr->make(&master, "bullet.player.small",
          master.phys.get_pos());
        // замедлить эти пули
        it->phys.set_speed(it->phys.get_speed() * rndr(0.5, 1));
        // чтобы пули разлетались во все стороны
        it->phys.set_deg(it->phys.get_deg() + rndr(-45, 45));
        it->phys.set_force( 7.5_pps );
        it->move_update_callback( Kill_by_timeout(rndr(0.1, 0.7)) );
        it->status.layer_up = false;
      } // if timer
    }; // ret lambda
  } // spawn_small_bullets

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
