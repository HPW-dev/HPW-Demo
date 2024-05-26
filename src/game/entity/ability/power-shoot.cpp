#include <cassert>
#include <typeinfo>
#include "power-shoot.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-archive.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "util/math/random.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "util/file/archive.hpp"
#include "util/math/timer.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"

template <typename T>
struct Minmax { T min {}, max {}; };

struct Ability_power_shoot::Impl {
  nocopy(Impl);
  uint m_power {1}; // сила эффекта
  hp_t m_price {}; // цена мощного выстрела
  hp_t m_energy_needed {}; // сколько должно быть энергии для мощного выстрела
  Str m_big_bullet {}; // ведущий снаряд
  Str m_small_bullet {}; // осколок ведущего снаряда
  Minmax<real> m_big_bullet_speed {};
  Minmax<real> m_small_bullet_speed {};
  Minmax<int> m_big_bullet_count {};
  Minmax<int> m_small_bullet_count {};
  Minmax<real> m_small_bullet_lifetime {}; // время существования осколков
  real m_small_bullet_delay {}; // с какой задержкой спавнить осколки ведущего снаряда
  real m_scatter_range {};
  real m_scatter_power {};
  real m_big_bullet_angle {}; // угол разброса ведущих пуль
  real m_small_bullet_angle {}; // угол разброса ведущих пуль
  real m_small_bullet_force {};

  inline explicit Impl(CN<Player> player) {
    load_config(player);
    test_config();
  }

  inline void update(Player& player, const double dt) {
    if (is_pressed(hpw::keycode::shoot) && player.energy >= m_energy_needed) {
      player.sub_en(m_price);
      make_bullets(player);
      make_scatter(player);
    }
  }

  inline void power_up() {
    ++m_power;
    // TODO заменять обычные осколки на взрывающиеся
  }

  inline utf32 name() const { return get_locale_str("plyaer.ability.power_shoot.name"); }

  inline utf32 desc() const {
    switch (m_power) {
      case 1: return get_locale_str("plyaer.ability.power_shoot.desc_0"); break;
      case 2: return get_locale_str("plyaer.ability.power_shoot.desc_1"); break;
      case 3: return get_locale_str("plyaer.ability.power_shoot.desc_2"); break;
      default:
      case 4: return get_locale_str("plyaer.ability.power_shoot.desc_3"); break;
    }
    return {};
  }

  inline void load_config(CN<Player> player) {
    assert(hpw::archive);
    cauto config_file = hpw::archive->get_file("config/ability.yml");
    cauto config = Yaml(config_file);
    cauto root = config["power_shoot"];
    assert(root.check());

    assert(player.energy_max);
    m_energy_needed = player.energy_max * (root.get_real("percent_energy_needed") / 100.0);
    m_price = player.energy_max * (root.get_real("percent_energy_price") / 100.0);

    // опции ведущего снаряда
    cauto big_bullet_node = root["big_bullet"];
    assert(big_bullet_node.check());
    m_big_bullet = big_bullet_node.get_str("name");
    cauto big_bullet_speed = big_bullet_node.get_v_real("speed");
    m_big_bullet_speed = {
      .min = pps( big_bullet_speed.at(0) ),
      .max = pps( big_bullet_speed.at(1) )
    };
    cauto big_bullet_count = big_bullet_node.get_v_int("count");
    m_big_bullet_count = {
      .min = big_bullet_count.at(0),
      .max = big_bullet_count.at(1)
    };
    m_big_bullet_angle = big_bullet_node.get_real("angle");

    // опции осколков
    cauto small_bullet_node = root["small_bullet"];
    assert(small_bullet_node.check());
    m_small_bullet = small_bullet_node.get_str("name");
    cauto small_bullet_speed = small_bullet_node.get_v_real("speed");
    m_small_bullet_speed = {
      .min = pps( small_bullet_speed.at(0) ),
      .max = pps( small_bullet_speed.at(1) )
    };
    cauto small_bullet_count = small_bullet_node.get_v_int("count");
    m_small_bullet_count = {
      .min = small_bullet_count.at(0),
      .max = small_bullet_count.at(1)
    };
    cauto small_bullet_lifetime = small_bullet_node.get_v_real("lifetime");
    m_small_bullet_lifetime = {
      .min = small_bullet_lifetime.at(0),
      .max = small_bullet_lifetime.at(1)
    };
    m_small_bullet_delay = small_bullet_node.get_real("delay");
    m_small_bullet_angle = small_bullet_node.get_real("angle");
    m_small_bullet_force = pps( small_bullet_node.get_real("force") );

    // конфиг отдачи
    cauto scatter_node = root["scatter"];
    m_scatter_range = scatter_node.get_real("range");
    m_scatter_power = scatter_node.get_real("power");
  } // load_config

  inline void test_config() const {
    assert(m_price > 0);
    assert(m_energy_needed > 0);
    assert(m_small_bullet_force > 0);
    assert(!m_big_bullet.empty());
    assert(!m_small_bullet.empty());
    assert(m_big_bullet_speed.max >= m_big_bullet_speed.min);
    assert(m_small_bullet_speed.max >= m_small_bullet_speed.min);
    assert(m_big_bullet_count.max >= m_big_bullet_count.min);
    assert(m_small_bullet_count.max >= m_small_bullet_count.min);
    assert(m_small_bullet_lifetime.max >= m_small_bullet_lifetime.min);
    assert(m_small_bullet_delay > 0 && m_small_bullet_delay < 10);
    assert(m_scatter_range > 0 && m_scatter_range < 100'000);
    assert(m_scatter_power > 0 && m_scatter_power < 1'000'000);
  }

  // запустить ведущие снаряды
  inline void make_bullets(Player& player) {
    cauto big_bullets = rnd (
      (m_big_bullet_count.min * pow2(m_power)),
      m_big_bullet_count.max * pow2(m_power) );
    cfor (_, big_bullets) {
      cauto spawn_pos = player.phys.get_pos() + Vec(rndr(-7, 7), 0);
      auto bullet = hpw::entity_mgr->make(&player, m_big_bullet, spawn_pos);
      bullet->phys.set_deg(270); // пуля смотрит вверх в шмап моде
      // передача импульса для шмап мода
      cauto big_bullet_speed = rndr(m_big_bullet_speed.min, m_big_bullet_speed.max);
      bullet->phys.set_speed(big_bullet_speed);
      bullet->phys.set_vel(bullet->phys.get_vel() + player.phys.get_vel());
      // разброс
      cauto angle = rndr(-m_big_bullet_angle, m_big_bullet_angle);
      bullet->phys.set_deg(bullet->phys.get_deg() + angle);
      bullet->move_update_callback(Spawner_small_bullets(this));
      bullet->status.ignore_scatter = true;
    }
  } // make_bullets

  // отдача от выстрела
  inline void make_scatter(Player& player) {
    hpw::entity_mgr->add_scatter(Scatter {
      .pos{ player.phys.get_pos() + Vec(0, -10) }, // создаёт источник взрыва перед ноосом
      .range = m_scatter_range * m_power,
      .power = pps(m_scatter_power) * m_power,
    });
  }

  // спавнер осколков для ведущего снаряда
  struct Spawner_small_bullets final {
    CP<Impl> m_master {};
    Timer m_delay {}; // задержка спавна

    inline explicit Spawner_small_bullets(CP<Impl> master): m_master {master} {
      m_delay = Timer(m_master->m_small_bullet_delay);
      m_delay.randomize_stable();
    }

    inline void operator()(Entity& entity, double dt) {
      cfor (_, m_delay.update(dt)) {
        auto it = hpw::entity_mgr->make(&entity, m_master->m_small_bullet,
          entity.phys.get_pos());
        // угол разброса
        cauto angle = rndr (
          -m_master->m_small_bullet_angle,
          m_master->m_small_bullet_angle );
        // сохранить импульс игрока
        cauto speed = rndr (
          m_master->m_small_bullet_speed.min * pow2(m_master->m_power),
          m_master->m_small_bullet_speed.max * pow2(m_master->m_power) );
        Vec vel = deg_to_vec(angle) * speed;
        it->phys.set_vel(it->phys.get_vel() + vel);
        it->phys.set_force(m_master->m_small_bullet_force);
        cauto kill_timeout = rndr (
          m_master->m_small_bullet_lifetime.min,
          m_master->m_small_bullet_lifetime.max
        );
        it->move_update_callback( Kill_by_timeout(kill_timeout) );
        it->status.layer_up = false;
      }
    } // op ()
  }; // Spawner_small_bullets
}; // Impl

Ability_power_shoot::Ability_power_shoot(CN<Player> player)
  : Ability {typeid(Ability_power_shoot).hash_code()}
  , impl {new_unique<Impl>(player)} {}
Ability_power_shoot::~Ability_power_shoot() {}
void Ability_power_shoot::update(Player& player, const double dt) { impl->update(player, dt); }
void Ability_power_shoot::power_up() { impl->power_up(); }
utf32 Ability_power_shoot::name() const { return impl->name(); }
utf32 Ability_power_shoot::desc() const { return impl->desc(); }
