#include "level-empty.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/core/tasks.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "game/util/task-util.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/hpw-util.hpp"

// #include "game/core/sounds.hpp"
// #include "util/math/vec-util.hpp"
// #include "util/math/timer.hpp"
// #include "util/math/random.hpp"
// #include "util/error.hpp"

struct Level_empty::Impl {
  Image screen {};

  inline explicit Impl() {
    set_default_collider();
  }

  inline void update(const Vec vel, Delta_time dt) {}

  inline void draw(Image& dst) const {}
}; // Impl

Level_empty::Level_empty(): impl {new_unique<Impl>()} {}
Level_empty::~Level_empty() {}
void Level_empty::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_empty::draw(Image& dst) const { impl->draw(dst); }
Str Level_empty::level_name() const { return Str{Level_empty::NAME}; }
