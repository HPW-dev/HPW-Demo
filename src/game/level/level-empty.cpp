#include "level-empty.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/collider/collider-grid.hpp"
#include "util/hpw-util.hpp"
#include "util/math/timer.hpp"

struct Level_empty::Impl {
  inline explicit Impl() {
    hpw::entity_mgr->set_collider(new_shared<Collider_grid>());
  }

  inline void update(const Vec vel, Delta_time dt) {
    // ...
  }

  inline void draw(Image& dst) const {
    dst.fill( Pal8::from_real(1.0 / 3.0, true) );
  }
}; // Impl

Level_empty::Level_empty(): impl {new_unique<Impl>()} {}
Level_empty::~Level_empty() {}
void Level_empty::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_empty::draw(Image& dst) const { impl->draw(dst); }
Str Level_empty::level_name() const { return Str{Level_empty::NAME}; }
