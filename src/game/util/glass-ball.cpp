#include <cassert>
#include <cmath>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/core/canvas.hpp"
#include "game/util/vec-helper.hpp"
#include "util/hpw-util.hpp"
#include "util/math/timer.hpp"

class Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _vel {};
  int _W {};
  int _H {};
  constx real START_SPEED {150.0_pps}; // с какой скоростью пулять шар
  constx real AIR_FORCE {1.2};         // сопротивление воздуха
  constx real G {9.81_pps};            // ускорение падения
  constx real MASS {30};               // массша шара
  constx real BOUND_FORCE {0.7};       // замедление шара при столкновении с краями экрана
  constx real SPEED_THRESHOLD {4};     // ниже этой скорости будет полная остановка шарика
  constx real GROUND_FORCE {20};       // сопротивление от трения
  Timer _respawn_timer {3};            // время до респавка шара, если о стоит

  inline bool _process_bounds() {
    bool collided = false;
    if (_pos.x < 0) {
      _pos.x *= -1;
      _vel.x *= -(1.0 - BOUND_FORCE);
      collided = true;
    }
    if (_pos.x >= _W) {
      _pos.x = _W - (_pos.x - _W);
      _vel.x *= -(1.0 - BOUND_FORCE);
      collided = true;
    }
    if (_pos.y < 0) {
      _pos.y *= -1;
      _vel.y *= -(1.0 - BOUND_FORCE);
      collided = true;
    }
    if (_pos.y >= _H) {
      _pos.y = _H - (_pos.y - _H);
      _vel.y *= -(1.0 - BOUND_FORCE);
      collided = true;
    }
    return collided;
  }

  inline void _respawn() {
    _pos = get_rand_pos_graphic(0, 0, _W, _H);
    _vel = rand_normalized_graphic() * START_SPEED * MASS;
    _respawn_timer.reset();
  }

public:
  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
    _W = graphic::width - _spr->X();
    _H = graphic::height - _spr->Y();
    _respawn();
  }

  inline void update(Delta_time dt) {
    const Vec FORCE_AIR = _vel * -AIR_FORCE;
    const Vec FORCE_GRAVITY(0, G * MASS);
    Vec ground_force {};
    if (std::abs(_vel.y) <= 1.5 && _pos.y >= _H-2)
      ground_force = _vel * -GROUND_FORCE;
    const Vec ACCEL = (FORCE_AIR + FORCE_GRAVITY + ground_force) / MASS;
    _vel += ACCEL * dt;
    _pos += _vel * dt;
    _process_bounds();

    // если шар слишком замедлился, то его пора респавнить
    if (length(_vel) <= SPEED_THRESHOLD) {
      if (_respawn_timer.update(dt))
        _respawn();
    }
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