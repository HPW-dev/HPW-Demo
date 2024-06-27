#pragma once
#include <mutex>
#include <functional>
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "util/str.hpp"
#include "util/unicode.hpp"

class Menu;
class Sprite;

// стартовое меню игры
class Scene_main_menu final: public Scene {
  Unique<Menu> menu {};
  Unique<Sprite> logo {};
  Vec logo_pos {};
  Strs m_logo_names {}; // пути к картинкам для лого
  std::once_flag m_logo_load_once {};
  Delta_time bg_state {}; // чтобы менять узор на фоне
  // функция рисующая фон
  std::function<void (Image& dst, const int bg_state)> bg_pattern_pf {};
  Timer change_bg_timer {12}; // таймер сменяющий фон

  void draw_bg(Image& dst) const;
  void init_menu();
  void init_logo();
  void init_bg();
  void draw_logo(Image& dst) const;
  void draw_text(Image& dst) const;
  void draw_wnd(Image& dst) const;
  void cache_logo_names();
  void next_bg();
  Unique<Sprite> prepare_logo(CN<Str> name) const;
  utf32 prepare_game_ver() const;
  void init_menu_sounds();
  void update_bg_order(const Delta_time dt);
  
public:
  Scene_main_menu();
  ~Scene_main_menu();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
