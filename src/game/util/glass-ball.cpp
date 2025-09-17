#include <cassert>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/core/canvas.hpp"
#include "util/hpw-util.hpp"

class Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _vel {};
  constx real START_SPEED {0.9};  // с какой скоростью пулять шар
  constx real AIR_FORCE {0.9};    // сопротивление воздуха
  constx real G {9};            // ускорение падения
  constx real MASS {10};          // массша шара
  constx real BOUND_FORCE {0.5};  // замедление шара при столкновении с краями экрана

  inline void _process_bounds() {
    cauto W = graphic::width - _spr->X();
    cauto H = graphic::height - _spr->Y();
    if (_pos.x < 0) {
      _pos.x *= -1;
      _vel.x = -_vel.x * BOUND_FORCE;
    }
    if (_pos.x >= W) {
      _pos.x = W - (_pos.x - W);
      _vel.x = -_vel.x * BOUND_FORCE;
    }
    if (_pos.y < 0) {
      _pos.y *= -1;
      _vel.y = -_vel.y * BOUND_FORCE;
    }
    if (_pos.y >= H) {
      _pos.y *= H - (_pos.y - H);
      _vel.y = -_vel.y * BOUND_FORCE;
    }
  }

public:
  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
  }

  inline void update(Delta_time dt) {
    Vec accel(0, MASS * G);
    accel += _vel * AIR_FORCE * -1;
    _vel += (accel / MASS) * dt;
    _pos += _vel * dt;
    _process_bounds();
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