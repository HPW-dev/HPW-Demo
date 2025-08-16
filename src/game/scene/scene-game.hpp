#pragma once
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"

// сцена игрового процесса
class Scene_game final: public Scene {
public:
  explicit Scene_game(const bool start_tutorial=false);
  ~Scene_game();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  constx Str NAME = "game";
  inline Str name() const override { return NAME; }

private:
  bool m_start_tutorial {false};
  struct Impl;
  Unique<Impl> m_impl {};
  
  void init_levels();
  void init_entitys();
  void replay_init();
  void post_draw(Image& dst) const;
  void draw_border(Image& dst) const; // рамка по краям экрана
  Vec get_level_vel() const; // безопасно получить сдвиг кординат уровня
  void save_named_replay();
  void startup_script();
  
};
