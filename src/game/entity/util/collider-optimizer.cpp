#include "collider-optimizer.hpp"
#include "game/core/common.hpp"
#include "game/entity/entity-mgr.hpp"

struct Collider_optimizer::Impl {
  Entity_mgr& m_entity_mgr;

  explicit inline Impl(Entity_mgr& entity_mgr): m_entity_mgr {entity_mgr} {}
  
  inline void update() {
    return_if (!hpw::collider_autoopt);


  }
}; // Impl

Collider_optimizer::Collider_optimizer(Entity_mgr& entity_mgr): impl{new_unique<Impl>(entity_mgr)} {}
Collider_optimizer::~Collider_optimizer() {}
void Collider_optimizer::update() { impl->update(); }
