#pragma once

/// флаги статуса в объекте Entity Status
struct Enity_status {
  bool live: 1 {};
  bool collidable: 1 {}; /// можно сталкиваться
  bool collided: 1 {}; /// минимум один раз в объект врезались
  bool killed: 1 {}; /// используется в update_kills для отложенной смерти
  bool kill_by_end_anim: 1 {};
  bool kill_by_timeout: 1 {};
  bool is_bullet: 1 {};
  bool is_enemy: 1 {};
  bool end_anim: 1 {};
  bool end_frame: 1 {}; /// set 1 if animation frame delay is end
  bool layer_up: 1 {false}; /// draw in upper level if == 1
  bool rnd_frame: 1 {}; /// каждый кадр анимации будет случайным
  bool rnd_deg: 1 {}; /// каждый кадр анимации будет случайно повёрнут (не каждый игровой кадр)
  bool rnd_deg_evr_frame: 1 {}; /// каждый игровой кадр, у анимации будет случайный угол
  bool stop_anim: 1 {}; /// if 1, no playing animation frames
  bool ignore_player: 1 {}; /// не сталкиваться с игроками
  bool ignore_master: 1 {}; /// не сталкиваться с создателем
  bool ignore_bullet: 1 {}; /// не сталкиваться с другими пулями
  bool ignore_self_type: 1 {}; /// не сталкиваться с объектами своего типа
  bool ignore_bound: 1 {}; /// не удалять объект, если он вышел за края экрана
  bool fixed_deg: 1 {false}; /// будет искать в anim_ctx default_deg и поворачивать только на него
  bool return_back: 1 {false}; /// включит флаг goto_prev_frame в кноце анимации
  bool goto_prev_frame: 1 {false}; /// отображает кадры в обратном порядке (дойдя до 0, выключается)
  bool no_motion_interp: 1 {false}; /// выключает интерполяцию при движении
  bool ignore_scatter: 1 {false}; /// игнор взрывных волнц
  bool ignore_enemy: 1 {false}; /// игнор других противников
  bool disable_contour: 1 {false}; /// выключает отображение контура, если он есть
  bool disable_heat_distort: 1 {false}; /// выключает отображение искажений воздуха
  bool disable_light: 1 {false}; /// выключает отображение выспышки
  bool no_restart_anim: 1 {false}; /// не перезапускать анимацию после окончания проигрывания
  bool disable_anim: 1 {false}; /// выключает отображение анимации
}; // Enity_status

constexpr auto ENITY_STATUS_BYTES {sizeof(Enity_status)};
