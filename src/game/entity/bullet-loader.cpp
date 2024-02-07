#include "bullet-loader.hpp"
#include "util/file/yaml.hpp"
#include "game/game-common.hpp"
#include "game/entity/util/info/anim-info.hpp"
#include "game/entity/util/info/collidable-info.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/entity-manager.hpp"

struct Bullet_loader::Impl {
  Collidable_info m_collidable_info {};
  Anim_info m_anim_info {};

  inline explicit Impl(CN<Yaml> config) {
    m_collidable_info.load(config);
    m_anim_info.load(config["animation"]);
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    auto it = hpw::entity_mgr->allocate<Collidable>();
    Entity_loader::prepare(*it, master, pos);
    m_collidable_info.accept(*it);
    m_anim_info.accept(*it);
    it->status.is_bullet = true;
    return it;
  } // op ()
   
}; // Impl

Bullet_loader::Bullet_loader(CN<Yaml> config): impl {new_unique<Impl>(config)} {}
Bullet_loader::~Bullet_loader() {}
Entity* Bullet_loader::operator()(Entity* master, const Vec pos, Entity* parent) { return impl->operator()(master, pos, parent); }
