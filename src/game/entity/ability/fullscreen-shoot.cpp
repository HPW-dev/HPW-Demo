#include <cassert>
#include <typeinfo>
#include <algorithm>
#include "fullscreen-shoot.hpp"
#include "game/entity/player.hpp"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/core/entities.hpp"
#include "util/math/timer.hpp"
#include "util/math/random.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"

struct Ability_fullscreen_shoot::Impl {
  nocopy(Impl);
  real m_clone_chance {0.05}; // вероятность, с которой будет создан клон пули
  real m_mul_clone_chance {2.333}; // на сколько усилить к m_clone_chance при паверапе
  real m_split_deg {17}; // в пределах какого угла будет расщепление пуль
  // если пуля близка к этой скорости, то шанс расщепления выше
  real m_high_speed_threshold {30_pps};
  // если пуль станет много на экране, то они все уничтожатся
  uint m_max_bullets {1'000};
  real m_mul_max_bullets {0.6}; // как менять m_max_bullets при паверапе
  bool m_need_clear {}; // true - уничтожить все пули
  uint m_power {};

  inline explicit Impl(CN<Player> player) {
    load_config();
  }

  inline void update(Player& player, const Delta_time dt) {
    uint bullets = 0;

    // попытаться склонировать все пули в игре, если удача прокнет
    nauto entities = hpw::entity_mgr->get_entities();
    for (nauto entity: entities) {
      assert(entity);
      cont_if( !(entity->status.live && entity->status.is_bullet) );
      ++bullets; // учесть пулю, которая уже была
      
      cont_if(check_bullet_chance(*entity));
      clone_bullet(*entity);
      ++bullets; // добавить клонированную пулю

      // если слишком много пуль, то больше не делать
      if (bullets > m_max_bullets) {
        m_need_clear = true;
        break;
      }
    }

    test_bullet_limit();
  } // update

  inline void power_up() {
    ++m_power;
    m_clone_chance *= m_mul_clone_chance;
    m_max_bullets *= m_mul_max_bullets;
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

  inline CP<Sprite> icon() const {
    return {}; // TODO
  }

  inline void clone_bullet(Entity& bullet) {
    // создать копию пули и добавить отклонение
    auto clone = hpw::entity_mgr->make(&bullet, bullet.name(), bullet.phys.get_pos());
    cauto split_deg = rndr(0, m_split_deg);
    clone->phys.set_deg(clone->phys.get_deg() + split_deg);
    bullet.phys.set_deg(bullet.phys.get_deg() - split_deg);
  }

  // чем медленнее объект, тем чаще будет выпадать true 
  inline bool check_bullet_chance(CN<Entity> bullet) const {
    cauto speed_ratio = std::min<real>(
      bullet.phys.get_speed() / m_high_speed_threshold, 1);
    return rndr() >= m_clone_chance * speed_ratio;
  }

  inline void test_bullet_limit() {
    // уничтожить пули если их много
    nauto entities = hpw::entity_mgr->get_entities();
    if (m_need_clear) {
      m_need_clear = false;
      for (nauto entity: entities) {
        assert(entity);
        cont_if( !(entity->status.live && entity->status.is_bullet) );
        entity->kill();
      }
    }
  }

  inline void load_config() {
    assert(hpw::archive);
    cauto config_file = hpw::archive->get_file("config/ability.yml");
    cauto config = Yaml(config_file);
    cauto root = config["fullscreen_shoot"];
    m_clone_chance         = root.get_real("clone_chance");
    m_mul_clone_chance     = root.get_real("mul_clone_chance");
    m_split_deg            = root.get_real("split_deg");
    m_high_speed_threshold = pps( root.get_real("high_speed_threshold") );
    m_max_bullets          = root.get_int("max_bullets");
    m_mul_max_bullets      = root.get_real("mul_max_bullets");
    test_params();
  }

  inline void test_params() {
    assert(m_clone_chance >= 0 && m_clone_chance <= 1);
    assert(m_split_deg >= 0 && m_split_deg <= 360);
    assert(m_high_speed_threshold >= 0 && m_high_speed_threshold <= 100'000_pps);
    assert(m_max_bullets >= 100 && m_max_bullets <= 4'000'000);
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
