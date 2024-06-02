#include "level-debug-4.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/player-dark.hpp"
#include "game/entity/util/anim-ctx.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/particle.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/level/level-manager.hpp"

struct Level_debug_4::Impl {
  inline explicit Impl() {
    make_player();
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(
      7, 1, graphic::canvas->X, graphic::canvas->Y));
  }

  inline void update(const Vec vel, double dt) {
    cauto player = hpw::entity_mgr->get_player();
    return_if( !player);
  }

  inline void draw(Image& dst) const {
    dst.fill( Pal8::from_real(1.0 / 3.0, true) );
  }

  void make_player() {
    auto player_pos_from_prev_level = hpw::level_mgr->player_prev_lvl_pos();
    // если игрок где-то стоял до этого, то поставить его туда же, иначе в центр экрана
    auto pos = player_pos_from_prev_level.not_zero() ? player_pos_from_prev_level : get_screen_center();
    hpw::entity_mgr->make({}, "player.boo.dark", pos);
  }
}; // Impl

Level_debug_4::Level_debug_4(): impl {new_unique<Impl>()} {}
Level_debug_4::~Level_debug_4() {}
void Level_debug_4::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_debug_4::draw(Image& dst) const { impl->draw(dst); }
Str Level_debug_4::level_name() const { return "Test level 4"; }
