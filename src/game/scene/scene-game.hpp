#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"

struct Vec;
class Replay;

// сцена игрового процесса
class Scene_game final: public Scene {
  Unique<Replay> replay {};
  bool m_start_tutorial {false};

  void draw_debug_info(Image& dst) const;
  void init_levels();
  void init_entitys();
  void replay_init();
  void replay_save_keys();
  void replay_load_keys();
  void post_draw(Image& dst) const;
  void draw_border(Image& dst) const; // рамка по краям экрана
  Vec get_level_vel() const; // безопасно получить сдвиг кординат уровня
  void save_named_replay();
  void startup_script();

public:
  explicit Scene_game(const bool start_tutorial=false);
  ~Scene_game();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
