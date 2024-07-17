#pragma once

// флаги управления и статуса игрового объекта
struct Entity_status {
  bool live: 1 {};             // объект жив и активен
  bool collidable: 1 {};       // можно сталкиваться
  bool collided: 1 {};         // минимум один раз в объект врезались
  bool killed: 1 {};           // используется в update_kills для отложенной смерти
  bool kill_by_end_anim: 1 {}; // убить объект концом анимации
  bool kill_by_end_frame: 1 {}; // убить объект в конце его кадра анимации
  bool kill_by_timeout: 1 {};  // убить объект концом времени жизни
  bool is_bullet: 1 {};        // объект является пулей
  bool is_enemy: 1 {};         // объект является врагом
  bool is_player: 1 {};        // объект является игроком
  bool end_anim: 1 {};         // анимация проигралась до конца
  bool end_frame: 1 {};        // кадр проигрался до конца
  bool layer_up: 1 {};         // рисовать объект на верхнем уровне
  bool rnd_frame: 1 {};        // каждый кадр анимации будет случайным
  bool rnd_deg: 1 {};          // каждый кадр анимации будет случайно повёрнут (не каждый игровой кадр)
  bool rnd_deg_evr_frame: 1 {}; // каждый игровой кадр, у анимации будет случайный угол
  bool stop_anim: 1 {};        // выключить проигрывание анимации
  bool ignore_player: 1 {};    // не сталкиваться с игроками
  bool ignore_master: 1 {};    // не сталкиваться с создателем
  bool ignore_bullet: 1 {};    // не сталкиваться с другими пулями
  bool ignore_self_type: 1 {}; // не сталкиваться с объектами своего типа
  bool ignore_bound: 1 {};     // не удалять объект, если он вышел за края экрана
  bool ignore_damage: 1 {}; // не получать урон
  bool fixed_deg: 1 {};        // будет искать в anim_ctx default_deg и поворачивать только на него
  bool return_back: 1 {};      // включит флаг goto_prev_frame в конце анимации
  bool goto_prev_frame: 1 {};  // отображает кадры в обратном порядке (дойдя до 0, выключается)
  bool no_motion_interp: 1 {}; // выключает интерполяцию при движении
  bool ignore_scatter: 1 {};   // игнор взрывных волн
  bool ignore_enemy: 1 {};     // игнор других противников
  bool disable_contour: 1 {};  // выключает отображение контура, если он есть
  bool disable_heat_distort: 1 {}; // выключает отображение искажений воздуха
  bool disable_light: 1 {};    // выключает отображение выспышки
  bool no_restart_anim: 1 {};  // не перезапускать анимацию после окончания проигрывания
  bool disable_render: 1 {};   // выключает отображение объекта и проигрывание анимации
  bool no_sound: 1 {};         // выключить звук объекта
  bool disable_motion: 1 {};   // запрет на перемещение
}; // Entity_status

inline constexpr auto ENITY_STATUS_BYTES {sizeof(Entity_status)};
