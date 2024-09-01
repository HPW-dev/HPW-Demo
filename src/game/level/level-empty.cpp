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
#include "util/math/timer.hpp"

struct Level_empty::Impl {
  inline explicit Impl() {
    set_default_collider();

    cauto kill_cb = [](Entity& src)
      { hpw_log("entity \"" << src.name() << "\" (id " << src.uid << ") killed\n"); };
    cauto remove_cb = [](Entity& src)
      { hpw_log("entity \"" << src.name() << "\" (id " << src.uid << ") removed\n"); };

    // постоять и умереть
    {
      auto ent = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center());
      ent->add_kill_cb(kill_cb);
      ent->add_remove_cb(remove_cb);
      hpw::task_mgr.add(new_shared<Task_timed>(5, [ent]{ ent->kill(); }));
    }

    // постоять и умереть (безопаснее)
    {
      auto ent = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center() - Vec(40, 0));
      cauto uid = ent->uid;
      ent->add_kill_cb(kill_cb);
      ent->add_remove_cb(remove_cb);
      hpw::task_mgr.add(new_shared<Task_timed>(4, [uid]{
        auto ptr = hpw::entity_mgr->get_entity(uid);
        if (ptr)
          ptr->kill();
      }));
    }
  
    // исчезнуть не через kill
    {
      auto ent = hpw::entity_mgr->make({}, "enemy.tutorial", get_screen_center() + Vec(40, 0));
      ent->add_kill_cb(kill_cb);
      ent->add_remove_cb(remove_cb);
      hpw::task_mgr.add(new_shared<Task_timed>(6, [ent]{ ent->remove(); }));
    }
  }

  inline void update(const Vec vel, Delta_time dt) {
    //...
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
