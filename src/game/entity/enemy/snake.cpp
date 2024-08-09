#include <cassert>
#include <utility>
#include "snake.hpp"
#include "util/hpw-util.hpp"
#include "util/file/yaml.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/effect/heat-distort.hpp"
#include "graphic/effect/heat-distort-util.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/core/anims.hpp"
#include "game/core/canvas.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/entity-util.hpp"

Enemy_snake_head::Enemy_snake_head(): Proto_enemy(GET_SELF_TYPE) {}
Enemy_snake_tail::Enemy_snake_tail(): Proto_enemy(GET_SELF_TYPE) {}

void Enemy_snake_head::update(const Delta_time dt) {
  Proto_enemy::update(dt);
  phys.set_speed(m_info.speed);
  phys.set_deg( deg_to_target(*this, hpw::entity_mgr->target_for_enemy()) );
}

void Enemy_snake_head::kill() {
  Proto_enemy::kill();
  /* узнать что начало хвоста ещё живо и вырезать из него
  инфу о мастере, чтобы запустить цепочку смерти */
  return_if (!m_info.tail_entity);
  return_if (m_info.tail_entity->uid != m_info.tail_entity_uid);
  m_info.tail_entity->set_master({});
}

void Enemy_snake_tail::update(const Delta_time dt) {
  Proto_enemy::update(dt);

  // хвост следует за создателем
  return_if(!master);
  return_if(!master->status.live);
  return_if(!status.ignore_scatter);

  // стоит ли хвост в пределах головы
  if (distance(this->phys.get_pos(), master->phys.get_pos())
  > m_info.start_motion_radius) {
    // взять скорость головы
    phys.set_speed( master->phys.get_speed() );
    phys.set_deg( deg_to_target(*this, *master) );
  } else { // если стоим в пределах головы, то не двигаться
    phys.set_speed(0);
  }
}

struct Enemy_snake_head::Loader::Impl {
  Info m_info {};

  inline explicit Impl(CN<Yaml> config) {
    m_info.speed = pps( config.get_real("speed") );

    cauto tail_node = config["tail"];
    m_info.tail_name = tail_node.get_str("name");
    m_info.tail_count = tail_node.get_int("count");
    m_info.kill_delay = tail_node.get_real("kill_delay", 0);
    m_info.enable_scatter_if_head_death = tail_node.get_bool("enable_scatter_if_head_death", false);

    assert( !m_info.tail_name.empty());
    assert(m_info.tail_count > 0 && m_info.tail_count < 100'000);
    assert(m_info.speed > 0);
    assert(m_info.kill_delay >= 0 && m_info.kill_delay <= 100'000);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Enemy_snake_head));
    auto it = ptr2ptr<Enemy_snake_head*>(parent);
    it->m_info = m_info;
    return parent;
  } // op ()
}; // Enemy_snake_head::Impl

struct Enemy_snake_tail::Loader::Impl {
  Info m_info {};

  inline explicit Impl(CN<Yaml> config) {;
    m_info.start_motion_radius = config.get_real("start_motion_radius");
    assert(m_info.start_motion_radius >= 0 && m_info.start_motion_radius <= 100'000);
  }

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Enemy_snake_tail));
    auto it = ptr2ptr<Enemy_snake_tail*>(parent);
    it->m_info = m_info;
    return parent;
  }
};

Entity* Enemy_snake_head::Loader::operator()
(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Enemy_snake_head>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);

  // понаделать хвостиков связанных по цепочке с головой
  Entity* last_tail = ret;
  cfor (i, ret->m_info.tail_count) {
    last_tail = hpw::entity_mgr->make(last_tail,
      ret->m_info.tail_name, ret->phys.get_pos());
    // записать в голову начало хвоста
    if (i == 0) {
      ret->m_info.tail_entity = last_tail;
      ret->m_info.tail_entity_uid = last_tail->uid;
    }
    // разлетается ли змейка при смерти
    if (ret->m_info.enable_scatter_if_head_death) {
      last_tail->add_update_callback([ret](Entity& it, const Delta_time dt) {
        return_if (ret && ret->status.live);
        it.status.ignore_scatter = false;
      }); 
    }
    // как должна умирать змейка
    if (ret->m_info.kill_delay) {
      last_tail->move_update_callback(
        std::move(Timed_kill_if_master_death(ret->m_info.kill_delay)) );
    } else {
      last_tail->add_update_callback(&kill_if_master_death);
    }
  }
  return ret;
} // Enemy_snake_head::Loader::op()

Entity* Enemy_snake_tail::Loader::operator()
(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Enemy_snake_tail>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Enemy_snake_head::Loader::Loader(CN<Yaml> config): Proto_enemy::Loader(config), impl{new_unique<Impl>(config)} {}
Enemy_snake_tail::Loader::Loader(CN<Yaml> config): Proto_enemy::Loader(config), impl{new_unique<Impl>(config)} {}
Enemy_snake_head::Loader::~Loader() {}
Enemy_snake_tail::Loader::~Loader() {}
