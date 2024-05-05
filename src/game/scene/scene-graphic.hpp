#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

class Menu;
class Menu_list_item;
class Menu_bool_item;
class Menu_int_item;
class Menu_list_item;
class Menu_text_item;

// меню настроек графики
class Scene_graphic final: public Scene {
  Shared<Menu> simple_menu {};
  Shared<Menu> detailed_menu {};
  Shared<Menu> preset_menu {};
  Weak<Menu> cur_menu {}; // переключает менюшки

  void init_simple_menu();
  void init_detailed_menu();
  void init_preset_menu();
  Shared<Menu_list_item> get_preset_item();
  Shared<Menu_bool_item> get_fullscreen_item();
  Shared<Menu_bool_item> get_vsync_item();
  Shared<Menu_bool_item> get_draw_border_item();
  Shared<Menu_bool_item> get_mouse_cursour_item();
  Shared<Menu_bool_item> get_disable_frame_limit_item();
  Shared<Menu_int_item> get_frame_limit_item();
  Shared<Menu_list_item> get_resize_type_item();
  Shared<Menu_text_item> get_goto_detailed_item();
  Shared<Menu_text_item> get_exit_item();
  Shared<Menu_text_item> get_palette_item();
  Shared<Menu_text_item> get_plugin_item();
  Shared<Menu_text_item> get_epilepsy_item();
  Shared<Menu_text_item> get_reset_item();
  Shared<Menu_text_item> get_gamma_item();

public:
  Scene_graphic();
  void update(double dt) override;
  void draw(Image& dst) const override;
};
