#include <cassert>
#include <utility>
#include "explosion-loader.hpp"
#include "util/math/vec.hpp"
#include "util/file/yaml.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/animation/anim.hpp"
#include "game/core/entities.hpp"
#include "game/entity/particle.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/heat-distort-util.hpp"
#include "graphic/effect/light.hpp"

struct Explosion_loader::Impl {
  Anim_info m_anim_info {};
  /* создаёт m_particle_count частиц из списка m_entity_names и раскидывает
  их по сторонам. Дальность раздёла - m_particles_range */
  real m_power {}; // pps
  real m_range {};
  real m_particles_range {}; // pps
  int m_particle_count {};
  Strs m_entity_names {};
  Heat_distort m_heat_distort {};

  inline explicit Impl(cr<Yaml> config) {
    m_anim_info.load(config["animation"]);

    m_power = config.get_real("power");
    m_range = config.get_real("range");
    m_particles_range = config.get_real("particles_range");
    m_particle_count = config.get_int("particle_count");
    assert(m_particle_count > 0);
    m_entity_names = config.get_v_str("names");
    assert(!m_entity_names.empty());
    if (auto heat_distort_node = config["heat_distort"]; heat_distort_node.check())
      m_heat_distort = load_heat_distort(heat_distort_node);
  }

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {  
    // расталкивание
    hpw::entity_mgr->add_scatter( std::move( Scatter{
      .pos {pos},
      .range {m_range},
      .power {pps(m_power)},
      .type {Scatter::Type::outside} // TODO выбор из скрипта
    }));

    // создать частицы
    cfor (particle_idx, m_particle_count) {
      // определить чё соспавнить
      auto entity_name = m_entity_names.at(rndu() % m_entity_names.size());
      auto it = hpw::entity_mgr->make(master, entity_name, pos);
      m_anim_info.accept(*it);
      // инит флагов
      it->status.ignore_self_type = true;
      it->status.ignore_master = true;
      // разлёт в случайную сторону, но с сохранением начального направления
      Vec motion = rand_normalized_stable() * rndr(0, pps(m_particles_range));
      it->phys.set_vel(it->phys.get_vel() + motion);
      init_shared(it->heat_distort, m_heat_distort);
      // TODO создание вспышки
    } // for m_particle_count
    
    /*
    auto it = hpw::entity_mgr->allocate<Particle>();
    Entity_loader::prepare(*it, master, pos);
    */
    return {}; // TODO от взрыва возвращать хитбокс
  } // op ()

}; // Impl

Explosion_loader::Explosion_loader(cr<Yaml> config)
: impl {new_unique<Impl>(config)}
{}

Explosion_loader::~Explosion_loader() {}

Entity* Explosion_loader::operator()(Entity* master, const Vec pos, Entity* parent)
{ return impl->operator()(master, pos, parent); }
