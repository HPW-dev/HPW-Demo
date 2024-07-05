#include "particle-loader.hpp"
#include "particle.hpp"
#include "util/entity-util.hpp"
#include "util/phys.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "util/math/vec.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"

struct Particle_loader::Impl {
  Anim_info m_anim_info {};
  bool m_kill_by_end_anim {}; // уничтожить частицу после конца анимации
  bool m_kill_by_end_frame {};
  bool m_rand_deg {}; // начальный угол сделает случайным
  real m_lifetime {}; // частица умрёт через время
  real m_force {};

  inline explicit Impl(CN<Yaml> config) {
    m_anim_info.load(config["animation"]);
    m_rand_deg          = config.get_bool("rand_deg");
    m_kill_by_end_anim  = config.get_bool("kill_by_end_anim", true);
    m_kill_by_end_frame = config.get_bool("kill_by_end_frame");
    m_lifetime          = config.get_real("lifetime");
    m_force             = config.get_real("force");
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    auto entity = hpw::entity_mgr->allocate<Particle>();
    Entity_loader::prepare(*entity, master, pos);
    
    m_anim_info.accept(*entity);
    entity->status.kill_by_end_anim = m_kill_by_end_anim;
    entity->status.kill_by_end_frame = m_kill_by_end_frame;
    entity->phys.set_force( pps(m_force) );
    if (m_lifetime > 0)
      entity->set_lifetime(m_lifetime);
    if (m_rand_deg) {
      entity->status.rnd_deg = true;
      entity->anim_ctx.set_default_deg(rand_degree_stable());
    }

    return entity;
  } // op ()
}; // Impl

Particle_loader::Particle_loader(CN<Yaml> config): impl{new_unique<Impl>(config)} {}
Particle_loader::~Particle_loader() {}
Entity* Particle_loader::operator()(Entity* master, const Vec pos, Entity* parent) { return impl->operator()(master, pos, parent); }