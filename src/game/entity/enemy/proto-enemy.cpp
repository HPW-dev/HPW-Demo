#include <cassert>
#include "proto-enemy.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "game/entity/util/info/collidable-info.hpp"
#include "game/entity/entity-mgr.hpp"
#include "util/file/yaml.hpp"

Proto_enemy::Proto_enemy(): Collidable(GET_SELF_TYPE)
{ status.is_enemy = true; }

Proto_enemy::Proto_enemy(Entity_type new_type)
: Proto_enemy()
{ type = new_type; }

struct Proto_enemy::Loader::Impl {
  Anim_info m_anim_info {};
  Collidable_info m_collidable_info {};

  inline explicit Impl(CN<Yaml> config) {
    m_collidable_info.load(config);
    m_anim_info.load(config["animation"]);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    auto it = ptr2ptr<Proto_enemy*>(parent);

    Entity_loader::prepare(*it, master, pos);
    m_anim_info.accept(*it);
    m_collidable_info.accept(*it);
    it->status.ignore_scatter = true;
    it->status.ignore_self_type = true;
    it->status.ignore_enemy = true;
    return parent;
  } // op ()

}; // Impl

Proto_enemy::Loader::Loader(CN<Yaml> config): impl{new_unique<Impl>(config)} {}
Proto_enemy::Loader::~Loader() {}
Entity* Proto_enemy::Loader::operator()(Entity* master, const Vec pos, Entity* parent) { return impl->operator()(master, pos, parent); }
