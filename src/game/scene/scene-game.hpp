#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/timer.hpp"

struct Vec;
class Replay;

/// сцена игрового процесса
class Scene_game final: public Scene {
  Unique<Replay> replay {};
  Timer death_timer {}; /// через это время засчитывается смерть игрока

  void draw_debug_info(Image& dst) const;
  void init_levels();
  void init_entitys();
  void replay_init();
  void replay_save_keys();
  void replay_load_keys();
  void check_death(double dt);
  void post_draw(Image& dst) const;
  void draw_border(Image& dst) const; /// рамка по краям экрана
  Vec get_level_vel() const; /// безопасно получить сдвиг кординат уровня

public:
  Scene_game();
  ~Scene_game();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
