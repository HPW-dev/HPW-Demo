#pragma once
#include <mutex>
#include <functional>
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"

class Menu;
class Sprite;

/// стартовое меню игры
class Scene_main_menu final: public Scene {
  Shared<Menu> menu {};
  Shared<Sprite> logo {};
  Vec logo_pos {};
  Strs m_logo_names {}; /// пути к картинкам для лого
  std::once_flag m_logo_load_once {};
  double bg_state {}; /// чтобы менять узор на фоне
  /// функция рисующая фон
  std::function<void (Image& dst, const int bg_state)> bg_pattern_pf {};
  Timer change_bg_timer {12}; /// таймер сменяющий фон

  void draw_bg(Image& dst) const;
  void init_menu();
  void init_logo();
  void init_bg();
  void draw_logo(Image& dst) const;
  void draw_text(Image& dst) const;
  void cache_logo_names();
  void next_bg();
  
public:
  Scene_main_menu();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
