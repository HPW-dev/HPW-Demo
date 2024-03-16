#include "level-tutorial.hpp"
#include "util/math/vec.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/core/levels.hpp"
#include "game/level/util/level-tasks.hpp"
#include "game/level/level-manager.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"

struct Level_tutorial::Impl {
  Level_tasks tasks {};

  inline explicit Impl() {
    hpw::shmup_mode = true;
    init_collider();
    make_player();
    init_tasks();
  }

  inline void update(const Vec vel, double dt) {
    execute_tasks(tasks, dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
  }

  inline void draw_upper_layer(Image& dst) const {}

  inline void make_player()
    { hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center()); }

  inline void init_collider() {
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(6, 1,
      graphic::width, graphic::height));
  }

  inline void init_tasks() {
    tasks = Level_tasks {
      [](double dt) {
        hpw::level_mgr->finalize_level();
        return true;
      },
    }; // Level_tasks c-tor
  } // init_tasks
}; // Impl

Level_tutorial::Level_tutorial(): impl {new_unique<Impl>()} {}
Level_tutorial::~Level_tutorial() {}
void Level_tutorial::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_tutorial::draw(Image& dst) const { impl->draw(dst); }
void Level_tutorial::draw_upper_layer(Image& dst) const { impl->draw_upper_layer(dst); }
