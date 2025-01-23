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
  Timer change_bg_timer {50}; // таймер сменяющий фон

  void init_menu();
  void init_logo();
  void draw_bg(Image& dst) const noexcept;
  void draw_logo(Image& dst) const noexcept;
  void draw_text(Image& dst) const noexcept;
  void draw_wnd(Image& dst) const noexcept;
  void cache_logo_names();
  void next_bg();
  Unique<Sprite> prepare_logo(cr<Str> name) const;
  utf32 prepare_game_ver() const;
  void init_menu_sounds();
  void update_bg_order(const Delta_time dt);
  
public:
  Scene_main_menu();
  ~Scene_main_menu();
  void update(const Delta_time dt) override final;
  void draw(Image& dst) const noexcept override final;
};
