#include <cassert>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "util/hpw-util.hpp"

struct Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _vel {};
  constx real START_SPEED {0.9};  // с какой скоростью пулять шар
  constx real AIR_FORCE {0.9};    // сопротивление воздуха
  constx real MASS {1};           // масса шарика
  constx real BOUND_FORCE {0.5};  // замедление шара при столкновении с краями экрана

  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
  }

  inline void update(Delta_time dt) {
    
  }

  inline void draw(Image& dst) const {
    assert(dst);
    insert(dst, *_spr, _pos);
  }
}; // Impl

Glass_ball::Glass_ball(): _impl{new_unique<Impl>()} {}
Glass_ball::~Glass_ball() {}
void Glass_ball::update(Delta_time dt) { _impl->update(dt); }
void Glass_ball::draw(Image& dst) const { _impl->draw(dst); }