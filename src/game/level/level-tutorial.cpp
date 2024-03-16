#include "level-tutorial.hpp"
#include "util/math/vec.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"

struct Level_tutorial::Impl {
  inline explicit Impl() {
    hpw::shmup_mode = true;
    init_collider();
    make_player();
  }

  inline void update(const Vec vel, double dt) {}

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
}; // Impl

Level_tutorial::Level_tutorial(): impl {new_unique<Impl>()} {}
Level_tutorial::~Level_tutorial() {}
void Level_tutorial::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_tutorial::draw(Image& dst) const { impl->draw(dst); }
void Level_tutorial::draw_upper_layer(Image& dst) const { impl->draw_upper_layer(dst); }
