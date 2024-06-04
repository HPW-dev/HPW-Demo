#include "level-empty.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"
#include "util/math/timer.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/core/levels.hpp"
#include "game/core/messages.hpp"
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

struct Level_empty::Impl {
  inline explicit Impl() {
    make_player();
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(
      7, 1, graphic::canvas->X, graphic::canvas->Y));
    
    // TODO del
    hpw::message_mgr->move(Message {
      .lifetime = 2,
      .text = U"test message 1"
    });
    hpw::message_mgr->move(Message {
      .lifetime = 4,
      .text = U"test message 2"
    });
    hpw::message_mgr->move(Message {
      .lifetime = 10,
      .text = U"blink fast",
      .blink_delay = 2
    });
    hpw::message_mgr->move(Message {
      .lifetime = 10,
      .text = U"blink slow",
      .blink_delay = 10
    });
  }

  inline void update(const Vec vel, Delta_time dt) {
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

Level_empty::Level_empty(): impl {new_unique<Impl>()} {}
Level_empty::~Level_empty() {}
void Level_empty::update(const Vec vel, Delta_time dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_empty::draw(Image& dst) const { impl->draw(dst); }
Str Level_empty::level_name() const { return "Test level 4"; }
