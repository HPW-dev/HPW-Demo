#pragma once
#include "util/macro.hpp"

namespace graphic {

  constx int FAST_FWD_UPD_SPDUP = 10; // во сколько раз ускорить апдейт при скипе
  constx int FAST_FWD_FRAMESKIP = 3; // сколько кадров пропускать при скипе
  
  inline bool wait_frame {true}; // ждать завершения отрисовки для стабильного апдейта
  inline bool step_mode {false}; // режим пошагового выполнения
  inline bool skip_cur_frame {false}; // пропустить отрисовку текущего кадра
  inline unsigned frame_count {}; // столько кадров прошло с начала игры
  inline double cur_fps {}; // текущий FPS
  inline double soft_draw_time {}; // время отрисовки графики движком
  inline double hard_draw_time {}; // время отрисовки графики системой

  // сеттеры приватных переменных:
  void set_vsync(bool val);
  void set_disable_frame_limit(bool val);
  void set_fast_forward(bool val);
  void set_target_fps(int val); // меняет лимит кадров
  void set_target_vsync_fps(int val);
  void set_target_frame_time(double val);
  void set_target_vsync_frame_time(double val);
  void set_bad_fps(double val);
  void set_draw_time_autoopt_limit(double val);

  // геттеры приватных переменных:
  bool get_vsync();
  bool get_disable_frame_limit();
  bool get_fast_forward();
  int get_target_fps();
  int get_target_vsync_fps();
  double get_target_frame_time();
  double get_target_vsync_frame_time();
  double get_bad_fps();
  double get_draw_time_autoopt_limit();
  
  void check_autoopt();

} // graphic ns
