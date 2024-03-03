#include <cassert>
#include <cmath>
#include <algorithm>
#include "anim-ctx.hpp"
#include "phys.hpp"
#include "game/entity/entity.hpp"
#include "game/core/graphic.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/error.hpp"
#include "util/math/random.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec-util.hpp"
#include "util/log.hpp"

Anim_ctx::Anim_ctx() {}

Anim_ctx::Anim_ctx(CN<decltype(anim)> new_anim)
: anim {new_anim}
{ assert(anim); }

void Anim_ctx::update(double dt, Entity &entity) {
  // если нет анимации, выйти
  if ( !anim) {
    entity.status.end_anim = true;
    return;
  }

  // не крутить анимацию, если не надо
  return_if (speed_scale == 0);
  return_if (entity.status.stop_anim);
  
  frame_timer += speed_scale * dt; // обновить таймер анимации

  // переключать кадры
  while (true) {
    auto frame = anim->get_frame(frame_idx);
    break_if( !frame);

    // если время текущего кадра не прошло, то ждать
    auto cur_duration = frame->duration;
    break_if (frame_timer < cur_duration);
    frame_timer -= cur_duration;

    update_frame_idx(entity);
    entity.status.end_frame = true;

    break_if(cur_duration == 0); // защита от зацикливания
  } // while 1
} // update

void Anim_ctx::update_frame_idx(Entity &entity) {
  auto frame_count = anim->frame_count();
  prev_frame_idx = frame_idx;

  // применение флагов на переключение кадров
  if (entity.status.rnd_frame) { // если след. кадр случайный
    frame_idx = rndu(frame_count-1);
  } else if (entity.status.goto_prev_frame) { // обратный порядок кадров
    prev_frame();
    /* дойдя до нуля включить обычный порядок
    и засчитать конец анимации */
    if (frame_idx == 0 && !entity.status.no_restart_anim) {
      entity.status.goto_prev_frame = false;
      entity.status.end_anim = true;
    }
  } else { // обычный порядок кадров
    ++frame_idx;
    // если дошли до конца:
    if (frame_idx >= frame_count) {
      if (entity.status.return_back) { // вернуться по кадрам обратно
        prev_frame();
        entity.status.goto_prev_frame = true;
      } else { // запустить анимацию заново
        if (entity.status.no_restart_anim)
          set_last_frame();
        else
          frame_idx = 0;
        entity.status.end_anim = true;
      }
    } // if end frame idx's
  }
} // next_frame

void Anim_ctx::draw(Image& dst, CN<Entity> entity, const Vec offset) {
  return_if(entity.status.disable_anim);

  // TODO сделать межкадровый дизеринг

  // взять текущий кадр анимации
  return_if( !anim);
  auto frame = anim->get_frame(frame_idx);
  return_if( !frame);

  // получить направление анимации
  auto degree = get_degree_with_flags(entity.phys.get_deg(), entity);
  if (entity.status.rnd_deg_evr_frame)
    degree = rand_degree_graphic();
  auto direct = frame->get_direct(degree);
  return_if( !direct);
  return_if(direct->sprite.expired());

  // вычислить корды вставки анимации
  draw_pos = entity.phys.get_pos();
  Vec contour_pos = draw_pos;

  /* если old_draw_pos нулевой, то с высокой вероятностью отрисовка
  происходит в первый раз, поэтому нужно задать хоть какое-то значение,
  иначе вся анимация размажется по экрану */
  if (!old_draw_pos)
    old_draw_pos = draw_pos;

  if (entity.status.no_motion_interp) { // рендер без интерполяции
    insert(dst, *direct->sprite.lock(), draw_pos + direct->offset + offset, blend_f, entity.uid);
    m_draw_pos = draw_pos;
  } else { 
    auto interpolated_pos = get_interpolated_pos();
    contour_pos = interpolated_pos;
    // то же, что и с old_draw_pos (см.выше)
    if (!old_interpolated_pos)
      old_interpolated_pos = interpolated_pos;

    if (graphic::enable_motion_blur) { // рендер с размытием
      insert_blured( dst, *direct->sprite.lock(),
        old_interpolated_pos + direct->offset + offset,
        interpolated_pos + direct->offset + offset, blend_f, entity.uid );
    } else { // обычный рендер
      insert(dst, *direct->sprite.lock(), interpolated_pos + direct->offset + offset, blend_f, entity.uid);
    }
    
    m_draw_pos = interpolated_pos;
    old_interpolated_pos = interpolated_pos;
  } // else motion interp

  old_draw_pos = draw_pos;
  if (!entity.status.disable_contour)
    draw_contour(dst, contour_pos + offset, degree);
} // draw

Vec Anim_ctx::get_interpolated_pos() const {
  return Vec (
    std::lerp<double>(old_draw_pos.x, draw_pos.x, graphic::lerp_alpha),
    std::lerp<double>(old_draw_pos.y, draw_pos.y, graphic::lerp_alpha)
  );
}

void Anim_ctx::set_anim(CN<decltype(anim)> new_anim) {
  *this = {};
  anim = new_anim;
}

void Anim_ctx::set_contour(CP<Anim> val) {
  assert(val);
  iferror(!anim, "anim не задан, поэтому контур может быть удалён при set_anim");
  contour = val;
}

void Anim_ctx::set_speed_scale(real new_scale)
  { speed_scale = std::clamp<real>(new_scale, 0, 30); }

void Anim_ctx::randomize_cur_frame_safe() {
  prev_frame_idx = frame_idx;
  frame_idx = rndu(anim->frame_count() - 1);
}

void Anim_ctx::randomize_cur_frame_graphic() {
  prev_frame_idx = frame_idx;
  frame_idx = rndu_fast(anim->frame_count() - 1);
}

real Anim_ctx::get_degree_with_flags(real src, CN<Entity> entity) const {
  auto status = entity.status;
  // если угол фиксирован
  if (status.fixed_deg)
    return fixed_deg;
  
  src = ring_deg(src + fixed_deg);
  // псевдослучайный угол
  if (status.rnd_deg || status.rnd_deg_evr_frame)
    return ring_deg(src + entity.uid * (360.0 / 16.0));
  return src;
}

void Anim_ctx::set_cur_frame(std::size_t num) {
  return_if ( !anim);
  cauto frame_count = anim->frame_count();
  prev_frame_idx = frame_idx;
  frame_idx = std::clamp<int>(num, 0, scast<int>(frame_count) - 1);
}

void Anim_ctx::set_last_frame() {
  return_if ( !anim);
  frame_idx = std::max<int>(0, scast<int>(anim->frame_count()) - 1);
}

CP<Frame> Anim_ctx::get_cur_frame() const {
  if (!anim)
    return nullptr;
  auto idx = get_cur_frame_idx();
  return anim->get_frame(idx);
}

CP<Hitbox> Anim_ctx::get_hitbox(real degree, CN<Entity> entity) const {
  if ( !anim)
    return nullptr;
  auto new_deg = get_degree_with_flags(degree, entity);
  return anim->get_hitbox(new_deg);
}

void Anim_ctx::update_hitbox(CN<Pool_ptr(Hitbox>) _hitbox) {
  // в режакторе можно менять константные вещи
  auto _anim = ccast<Anim*>(anim);
  if (_anim)
    _anim->update_hitbox(_hitbox);
  else
    error("hitbox set error");
} // update_hitbox

void Anim_ctx::set_default_deg(real deg) { fixed_deg = ring_deg(deg); }
decltype(Anim_ctx::fixed_deg) Anim_ctx::get_default_deg() const { return fixed_deg; }

void Anim_ctx::draw_contour(Image& dst, const Vec offset, real degree) const {
  return_if (!contour);
  auto contour_frame = contour->get_frame(frame_idx);
  return_if (!contour_frame);
  auto contour_direct = contour_frame->get_direct(degree);
  return_if (!contour_direct || contour_direct->sprite.expired());
  auto contour_sprite = contour_direct->sprite.lock();
  insert(dst, *contour_sprite, contour_direct->offset + offset, contour_bf);
} // draw_contour

Vec Anim_ctx::get_draw_pos() const { return m_draw_pos; }
