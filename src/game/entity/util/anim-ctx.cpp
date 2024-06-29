#include <cmath>
#include <cassert>
#include <algorithm>
#include "phys.hpp"
#include "anim-ctx.hpp"
#include "util/error.hpp"
#include "util/math/random.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec-util.hpp"
#include "game/core/graphic.hpp"
#include "game/entity/entity.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"

Anim_ctx::Anim_ctx(CP<Anim> new_anim): m_anim {new_anim}
  { assert(m_anim); }

void Anim_ctx::update(const Delta_time dt, Entity &entity) {
  // если нет анимации, выйти
  if ( !m_anim) {
    entity.status.end_anim = true;
    return;
  }

  // не крутить анимацию, если не надо
  return_if (m_speed_scale == 0);
  return_if (entity.status.stop_anim);
  
  m_frame_timer += m_speed_scale * dt; // обновить таймер анимации

  // переключать кадры
  while (true) {
    auto frame = m_anim->get_frame(m_frame_idx);
    break_if( !frame);

    // если время текущего кадра не прошло, то ждать
    auto cur_duration = frame->duration;
    break_if (m_frame_timer < cur_duration);
    m_frame_timer -= cur_duration;

    update_frame_idx(entity);
    entity.status.end_frame = true;

    break_if(cur_duration == 0); // защита от зацикливания
  } // while 1
} // update

void Anim_ctx::update_frame_idx(Entity &entity) {
  auto frame_count = m_anim->frame_count();
  m_prev_frame_idx = m_frame_idx;

  // применение флагов на переключение кадров
  if (entity.status.rnd_frame) { // если след. кадр случайный
    m_frame_idx = rndu(frame_count-1);
  } else if (entity.status.goto_prev_frame) { // обратный порядок кадров
    prev_frame();
    /* дойдя до нуля включить обычный порядок
    и засчитать конец анимации */
    if (m_frame_idx == 0 && !entity.status.no_restart_anim) {
      entity.status.goto_prev_frame = false;
      entity.status.end_anim = true;
    }
  } else { // обычный порядок кадров
    ++m_frame_idx;
    // если дошли до конца:
    if (m_frame_idx >= frame_count) {
      if (entity.status.return_back) { // вернуться по кадрам обратно
        prev_frame();
        entity.status.goto_prev_frame = true;
      } else { // запустить анимацию заново
        if (entity.status.no_restart_anim)
          set_last_frame();
        else
          m_frame_idx = 0;
        entity.status.end_anim = true;
      }
    } // if end frame idx's
  }
} // next_frame

void Anim_ctx::draw(Image& dst, CN<Entity> entity, const Vec offset) {
  // TODO сделать межкадровый дизеринг

  // взять текущий кадр анимации
  return_if( !m_anim);
  auto frame = m_anim->get_frame(m_frame_idx);
  return_if( !frame);

  // получить направление анимации
  auto degree = get_degree_with_flags(entity.phys.get_deg(), entity);
  if (entity.status.rnd_deg_evr_frame)
    degree = rand_degree_graphic();
  auto direct = frame->get_direct(degree);
  return_if( !direct);
  return_if(direct->sprite.expired());

  // вычислить корды вставки анимации
  m_draw_pos = entity.phys.get_pos();
  Vec contour_pos = m_draw_pos;

  /* если m_old_draw_pos нулевой, то с высокой вероятностью отрисовка
  происходит в первый раз, поэтому нужно задать хоть какое-то значение,
  иначе вся анимация размажется по экрану */
  if (!m_old_draw_pos.not_zero())
    m_old_draw_pos = m_draw_pos;

  // рендер без интерполяции
  if (entity.status.no_motion_interp || !graphic::enable_motion_interp) {
    insert(dst, *direct->sprite.lock(), m_draw_pos + direct->offset + offset,
      blend_f, entity.uid);
    m_drawed_pos = m_draw_pos;
  } else { // рендер с интерполяцией
    auto interp_pos = get_interpolated_pos();
    contour_pos = interp_pos;
    // то же, что и с m_old_draw_pos (см.выше)
    if (m_old_interp_pos.is_zero())
      m_old_interp_pos = interp_pos;

    if (graphic::enable_motion_blur) { // рендер с размытием
      insert_blured( dst, *direct->sprite.lock(),
        m_old_interp_pos + direct->offset + offset,
        interp_pos + direct->offset + offset, blend_f, entity.uid );
    } else { // обычный рендер
      insert(dst, *direct->sprite.lock(),
        interp_pos + direct->offset + offset, blend_f, entity.uid);
    }
    
    m_drawed_pos = interp_pos;
    m_old_interp_pos = interp_pos;
  } // else motion interp

  m_old_draw_pos = m_draw_pos;
  if (!entity.status.disable_contour)
    draw_contour(dst, contour_pos + offset, degree);
} // draw

Vec Anim_ctx::get_interpolated_pos() const {
  return Vec (
    std::lerp<Delta_time>(m_old_draw_pos.x, m_draw_pos.x, graphic::lerp_alpha),
    std::lerp<Delta_time>(m_old_draw_pos.y, m_draw_pos.y, graphic::lerp_alpha)
  );
}

void Anim_ctx::set_anim(CP<Anim> new_anim) {
  *this = {};
  m_anim = new_anim;
}

void Anim_ctx::set_contour(CP<Anim> val) {
  assert(val);
  iferror(!m_anim, "m_anim не задан, поэтому контур "
    "может быть удалён при set_anim");
  m_contour = val;
}

void Anim_ctx::set_speed_scale(real new_scale)
  { m_speed_scale = std::clamp<real>(new_scale, 0, 30); }

void Anim_ctx::randomize_cur_frame_safe() {
  m_prev_frame_idx = m_frame_idx;
  m_frame_idx = rndu(m_anim->frame_count() - 1);
}

real Anim_ctx::get_degree_with_flags(real src, CN<Entity> entity) const {
  auto status = entity.status;
  // если угол фиксирован
  if (status.fixed_deg)
    return m_fixed_deg;
  
  src = ring_deg(src + m_fixed_deg);
  // псевдослучайный угол
  if (status.rnd_deg || status.rnd_deg_evr_frame)
    return ring_deg(src + entity.uid * (360.0 / 16.0));
  return src;
}

void Anim_ctx::set_cur_frame(std::size_t num) {
  return_if ( !m_anim);
  cauto frame_count = m_anim->frame_count();
  m_prev_frame_idx = m_frame_idx;
  m_frame_idx = std::clamp<int>(num, 0, scast<int>(frame_count) - 1);
}

void Anim_ctx::set_last_frame() {
  return_if ( !m_anim);
  m_frame_idx = std::max<int>(0, scast<int>(m_anim->frame_count()) - 1);
}

CP<Frame> Anim_ctx::get_cur_frame() const {
  if (!m_anim)
    return nullptr;
  auto idx = get_cur_frame_idx();
  return m_anim->get_frame(idx);
}

CP<Hitbox> Anim_ctx::get_hitbox(real degree, CN<Entity> entity) const {
  if ( !m_anim)
    return nullptr;
  auto new_deg = get_degree_with_flags(degree, entity);
  return m_anim->get_hitbox(new_deg);
}

void Anim_ctx::update_hitbox(CN<Pool_ptr(Hitbox>) _hitbox) {
  // в режакторе можно менять константные вещи
  auto _anim = ccast<Anim*>(m_anim);
  if (_anim)
    _anim->update_hitbox(_hitbox);
  else
    error("hitbox set error");
} // update_hitbox

void Anim_ctx::set_default_deg(real deg) { m_fixed_deg = ring_deg(deg); }

void Anim_ctx::draw_contour(Image& dst, const Vec offset, real degree) const {
  return_if (!m_contour);
  auto contour_frame = m_contour->get_frame(m_frame_idx);
  return_if (!contour_frame);
  auto contour_direct = contour_frame->get_direct(degree);
  return_if (!contour_direct || contour_direct->sprite.expired());
  auto contour_sprite = contour_direct->sprite.lock();
  insert(dst, *contour_sprite, contour_direct->offset + offset, contour_bf);
} // draw_contour
