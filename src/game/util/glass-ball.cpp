#include <cassert>
#include <cmath>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/sprites.hpp"
#include "game/core/sounds.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/util/vec-helper.hpp"
#include "game/util/sound-helper.hpp"
#include "util/hpw-util.hpp"
#include "util/math/timer.hpp"
#include "util/math/random.hpp"
#include "util/rnd-table.hpp"

class Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _old_pos {};
  Vec _vel {};
  int _W {};
  int _H {};
  constx real START_SPEED {400.0_pps};    // с какой скоростью пулять шар
  constx real AIR_FORCE {0.3_pps};        // сопротивление воздуха
  constx real G {30.0_pps};               // ускорение падения
  constx real MASS {150};                 // массша шара
  constx real BOUND_FORCE {0.6};          // замедление шара при столкновении с краями экрана
  constx real GROUND_FORCE {0.01};        // сопротивление от трения
  constx real SPEED_THRESHOLD {5.0_pps};  // ниже этой скорости будет полная остановка шарика
  Timer _respawn_timer {1.9};             // время до респавка шара, если о стоит

  inline void _play_hit_sound() {
    sconst Rnd_table<Str> names {Rnd_table<Str>::Values{
      "sfx/hit/glass/1.flac",
      "sfx/hit/glass/2.flac",
      "sfx/hit/glass/3.flac",
      "sfx/hit/glass/4.flac",
    }};

    // TODO из файла с настройками брать звук шара
    hpw::sound_mgr->play(names.rnd_fast(), to_sound_pos(_pos), {}, 1.0);
  }

  inline bool _process_bounds() {
    bool collided = false;
    if (_pos.x < 0) {
      _pos.x *= -1;
      _vel.x *= -(1.0 - BOUND_FORCE);
      _vel.y *= (1.0 - GROUND_FORCE);
      _play_hit_sound();
      collided = true;
    }
    if (_pos.x >= _W) {
      _pos.x = _W - (_pos.x - _W);
      _vel.x *= -(1.0 - BOUND_FORCE);
      _vel.y *= (1.0 - GROUND_FORCE);
      _play_hit_sound();
      collided = true;
    }
    if (_pos.y < 0) {
      _pos.y *= -1;
      _vel.x *= (1.0 - GROUND_FORCE);
      _vel.y *= -(1.0 - BOUND_FORCE);
      _play_hit_sound();
      collided = true;
    }
    if (_pos.y >= _H) {
      _pos.y = _H - (_pos.y - _H);
      _vel.x *= (1.0 - GROUND_FORCE);
      _vel.y *= -(1.0 - BOUND_FORCE);
      if (std::abs(_vel.y) > 1.0_pps)
        _play_hit_sound();
      collided = true;
    }
    return collided;
  }

  inline void _respawn() {
    _old_pos = _pos = get_rand_pos_graphic(0, 0, _W, _H);
    _vel = rand_normalized_graphic() * START_SPEED;
    _respawn_timer.reset();
  }

public:
  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
    _W = graphic::width - _spr->X();
    _H = graphic::height - _spr->Y();
    _respawn();
    hpw::sound_mgr->set_listener_pos(to_sound_pos({_W/2.0, _H/2.0}));
  }

  inline void update(Delta_time dt) {
    // физика шарика
    const Vec FORCE_AIR = _vel * -AIR_FORCE;
    const Vec FORCE_GRAVITY(0, G * MASS);
    const Vec ACCEL = (FORCE_AIR + FORCE_GRAVITY) / MASS;
    _vel += ACCEL * dt;
    _old_pos = _pos;
    _pos += _vel * dt;

    _process_bounds();

    // если шар слишком замедлился, то его пора респавнить
    if (length(_vel) <= SPEED_THRESHOLD && _pos.y >= _H-2) {
      if (_respawn_timer.update(dt))
        _respawn();
    } else {
      _respawn_timer.reset();
    }
  }

  inline void draw(Image& dst) const {
    assert(dst);

    if (rndr_graphic() < _respawn_timer.ratio()) {
      //insert(dst, *_spr, _pos);
      if (graphic::motion_blur_mode != Motion_blur_mode::disabled) // рендер с блюром
        insert_blured(dst, *_spr, _old_pos, _pos);
      else // рендер без блюра
        insert(dst, *_spr, _pos);
    }
  }
}; // Impl

Glass_ball::Glass_ball(): _impl{new_unique<Impl>()} {}
Glass_ball::~Glass_ball() {}
void Glass_ball::update(Delta_time dt) { _impl->update(dt); }
void Glass_ball::draw(Image& dst) const { _impl->draw(dst); }