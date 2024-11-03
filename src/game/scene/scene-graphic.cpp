#include "scene-graphic.hpp"
#include "scene-palette-select.hpp"
#include "scene-pge.hpp"
#include "scene-mgr.hpp"
#include "host/command.hpp"
#include "host/host-util.hpp"
#include "game/core/common.hpp"
#include "game/core/debug.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core-window.hpp"
#include "game/core/fonts.hpp"
#include "game/core/graphic.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sounds.hpp"
#include "game/util/pge.hpp"
#include "game/util/config.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-gamma.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/int-item.hpp"
#include "game/menu/item/double-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "sound/sound-mgr.hpp"
#include "util/error.hpp"

Scene_graphic::Scene_graphic() {
  init_simple_menu();
  init_detailed_menu();
  init_preset_menu();
  cur_menu = simple_menu;
}

void Scene_graphic::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape)) {
    hpw::scene_mgr->back();
    save_config();
  }
  
  iferror (cur_menu.expired(), "menu ptr error");
  cur_menu.lock()->update(dt);
}

void Scene_graphic::draw(Image& dst) const {
  iferror (cur_menu.expired(), "menu ptr error");
  cur_menu.lock()->draw(dst);
}

void set_adaptive() {
  graphic::set_vsync(false);
  graphic::set_disable_frame_limit(false);
  graphic::set_target_fps(graphic::get_target_vsync_fps());
  graphic::autoopt_timeout_max = graphic::default_autoopt_timeout;
  graphic::blink_motion_blur = true;
  graphic::blink_particles = true;
  graphic::blur_quality_mul = 1.0;
  graphic::cpu_safe = false;
  graphic::disable_heat_distort_while_lag = true;
  graphic::wait_frame_bak = graphic::wait_frame = true;
  graphic::double_buffering = true;
  graphic::enable_heat_distort = false;
  graphic::enable_light = true;
  graphic::enable_motion_blur = true;
  graphic::light_quality = Light_quality::medium;
  graphic::motion_blur_quality_reduct = true;
  graphic::frame_skip = 3;
  graphic::auto_frame_skip = true;
  graphic::enable_motion_interp = true;
  #ifdef DEBUG
    graphic::fast_blur = true;
  #else
    graphic::fast_blur = false;
  #endif
}

void set_default() {
  #ifdef DEBUG
    set_adaptive();
  #else
    set_high_quality();
  #endif
}

void set_high_plus_stream() {
  graphic::set_vsync(false);
  graphic::set_disable_frame_limit(false);
  graphic::set_target_fps(graphic::get_target_vsync_fps() + 15);
  graphic::blink_motion_blur = false;
  graphic::blink_particles = false;
  graphic::blur_quality_mul = 0.5;
  graphic::cpu_safe = false;
  graphic::disable_heat_distort_while_lag = false;
  graphic::wait_frame_bak = graphic::wait_frame = false;
  graphic::double_buffering = true;
  graphic::enable_heat_distort = true;
  graphic::enable_light = true;
  graphic::enable_motion_blur = true;
  graphic::light_quality = Light_quality::high;
  graphic::motion_blur_quality_reduct = false;
  graphic::frame_skip = 1;
  graphic::auto_frame_skip = true;
  graphic::enable_motion_interp = true;
  graphic::fast_blur = false;
} // set_high_plus_stream

void set_low_pc() {
  graphic::set_vsync(false);
  graphic::set_disable_frame_limit(true);
  graphic::autoopt_timeout_max = 15;
  graphic::blink_particles = true;
  graphic::cpu_safe = false;
  graphic::wait_frame_bak = graphic::wait_frame = false;
  graphic::double_buffering = true;
  graphic::enable_heat_distort = false;
  graphic::enable_light = false;
  graphic::enable_motion_blur = false;
  graphic::light_quality = Light_quality::low;
  graphic::motion_blur_quality_reduct = true;
  graphic::frame_skip = 5;
  graphic::auto_frame_skip = true;
  graphic::enable_motion_interp = false;
  graphic::fast_blur = true;
} // set_low_pc

void set_high_quality() {
  graphic::set_vsync(true);
  graphic::set_disable_frame_limit(false);
  graphic::blink_motion_blur = false;
  graphic::blink_particles = false;
  graphic::blur_quality_mul = 0.5;
  graphic::cpu_safe = true;
  graphic::disable_heat_distort_while_lag = false;
  graphic::wait_frame_bak = graphic::wait_frame = true;
  graphic::double_buffering = true;
  graphic::enable_heat_distort = true;
  graphic::enable_light = true;
  graphic::enable_motion_blur = true;
  graphic::light_quality = Light_quality::high;
  graphic::motion_blur_quality_reduct = false;
  graphic::frame_skip = 0;
  graphic::auto_frame_skip = false;
  graphic::enable_motion_interp = true;
  graphic::fast_blur = false;
} // set_high_quality

Shared<Menu_list_item> Scene_graphic::get_preset_item() {
  return new_shared<Menu_list_item>(
    get_locale_str("scene.graphic_menu.pressets.name"),
    Menu_list_item::Items {
      Menu_list_item::Item {
        get_locale_str("scene.graphic_menu.pressets.default"),
        get_locale_str("scene.graphic_menu.description.pressets.default"),
        &set_default
      },
      Menu_list_item::Item {
        get_locale_str("scene.graphic_menu.pressets.low_pc"),
        get_locale_str("scene.graphic_menu.description.pressets.low_pc"),
        &set_low_pc
      },
      Menu_list_item::Item {
        get_locale_str("scene.graphic_menu.pressets.high_quality_plus_stream"),
        get_locale_str("scene.graphic_menu.description.pressets.high_quality_plus_stream"),
        &set_high_plus_stream
      },
      Menu_list_item::Item {
        get_locale_str("scene.graphic_menu.pressets.high_quality"),
        get_locale_str("scene.graphic_menu.description.pressets.high_quality"),
        &set_high_quality
      }
    } // Menu list items
  );
} // get_preset_item

Shared<Menu_bool_item> Scene_graphic::get_fullscreen_item() {
  return new_shared<Menu_bool_item>(
    get_locale_str("scene.graphic_menu.fullscreen"),
    []{ return graphic::fullscreen; },
    [](bool new_val) { hpw::set_fullscreen(new_val); },
    get_locale_str("scene.graphic_menu.description.fullscreen")
  );
}

Shared<Menu_bool_item> Scene_graphic::get_vsync_item() {
  return new_shared<Menu_bool_item>(
    get_locale_str("scene.graphic_menu.vsync"),
    []{ return graphic::get_vsync(); },
    [](bool new_val) { graphic::set_vsync(new_val); },
    get_locale_str("scene.graphic_menu.description.vsync")
  );
}

Shared<Menu_bool_item> Scene_graphic::get_draw_border_item() {
  return new_shared<Menu_bool_item>(
    get_locale_str("scene.graphic_menu.draw_border"),
    []{ return graphic::draw_border; },
    [](bool new_val) { graphic::draw_border = new_val; },
    get_locale_str("scene.graphic_menu.description.draw_border")
  );
}

Shared<Menu_bool_item> Scene_graphic::get_mouse_cursour_item() {
  return new_shared<Menu_bool_item>(
    get_locale_str("scene.graphic_menu.show_mouse_cursour"),
    []{ return graphic::show_mouse_cursour; },
    [](bool new_val) { hpw::set_mouse_cursour_mode(new_val); },
    get_locale_str("scene.graphic_menu.description.show_mouse_cursour")
  );
}

Shared<Menu_bool_item> Scene_graphic::get_disable_frame_limit_item() {
  return new_shared<Menu_bool_item>(
    get_locale_str("scene.graphic_menu.disable_frame_limit"),
    []{ return graphic::get_disable_frame_limit(); },
    [](bool new_val) { graphic::set_disable_frame_limit(new_val); },
    get_locale_str("scene.graphic_menu.description.disable_frame_limit")
  );
}

Shared<Menu_int_item> Scene_graphic::get_frame_limit_item() {
  return new_shared<Menu_int_item>(
    get_locale_str("scene.graphic_menu.frame_limit"),
    []{ return graphic::get_target_fps(); },
    [](int new_val) { graphic::set_target_fps(std::clamp(new_val, 10, 3'000)); },
    10,
    get_locale_str("scene.graphic_menu.description.frame_limit")
  );
}

Shared<Menu_list_item> Scene_graphic::get_resize_type_item() {
  return new_shared<Menu_list_item>(
    get_locale_str("scene.graphic_menu.resize_type.name"),
    Menu_list_item::Items {
      Menu_list_item::Item {
        .name = get_locale_str("scene.graphic_menu.resize_type.one_to_one"),
        .desc = get_locale_str("scene.graphic_menu.description.resize_type.one_to_one"),
        .action = []{ hpw::set_resize_mode(Resize_mode::one_to_one); }
      },
      Menu_list_item::Item {
        .name = get_locale_str("scene.graphic_menu.resize_type.x_only"),
        .desc = get_locale_str("scene.graphic_menu.description.resize_type.x_only"),
        .action = []{ hpw::set_resize_mode(Resize_mode::by_width); }
      },
      Menu_list_item::Item {
        .name = get_locale_str("scene.graphic_menu.resize_type.y_only"),
        .desc = get_locale_str("scene.graphic_menu.description.resize_type.y_only"),
        .action = []{ hpw::set_resize_mode(Resize_mode::by_height); }
      },
      Menu_list_item::Item {
        .name = get_locale_str("scene.graphic_menu.resize_type.full"),
        .desc = get_locale_str("scene.graphic_menu.description.resize_type.full"),
        .action = []{ hpw::set_resize_mode(Resize_mode::full); }
      }
    }, // items
    scast<std::size_t>(graphic::resize_mode)
  );
}

Shared<Menu_text_item> Scene_graphic::get_goto_detailed_item() {
  return new_shared<Menu_text_item> (
    get_locale_str("scene.graphic_menu.goto_detailed"),
    [this]{ cur_menu = detailed_menu; }
  );
}

Shared<Menu_text_item> Scene_graphic::get_exit_item() {
  return new_shared<Menu_text_item>(
    get_locale_str("common.exit"),
    []{
      hpw::scene_mgr->back();
      save_config();
    }
  );
}

Shared<Menu_text_item> Scene_graphic::get_palette_item() {
  return new_shared<Menu_text_item>(
    get_locale_str("scene.palette_select.title"),
    []{ hpw::scene_mgr->add(new_shared<Scene_palette_select>()); },
    []->utf32 { return {}; },
    get_locale_str("scene.graphic_menu.description.palette_select")
  );
}

Shared<Menu_text_item> Scene_graphic::get_plugin_item() {
  return new_shared<Menu_text_item>(
    get_locale_str("scene.graphic_menu.pge.title"),
    []{ hpw::scene_mgr->add(new_shared<Scene_pge>()); },
    []->utf32 { return {}; },
    get_locale_str("scene.graphic_menu.description.pge")
  );
}

Shared<Menu_text_item> Scene_graphic::get_epilepsy_item() {
  return new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.epilepsy"), [] {
    hpw::init_palette_from_archive("resource/image/palettes/zebura.png");
    hpw::set_resize_mode(Resize_mode::full);
    graphic::set_disable_frame_limit(true);
    #ifdef WINDOWS
      load_pge(hpw::cur_dir + hpw::plugin_path + "effect/epilepsy.dll");
    #else
      load_pge(hpw::cur_dir + hpw::plugin_path + "effect/libepilepsy.so");
    #endif
  }, []->utf32 { return {}; },
  get_locale_str("scene.graphic_menu.description.epilepsy") );
}

Shared<Menu_text_item> Scene_graphic::get_reset_item() {
  return new_shared<Menu_text_item>(get_locale_str("common.reset"), [] {
    hpw::init_palette_from_archive("resource/image/palettes/default.png");
    hpw::set_resize_mode(graphic::default_resize_mode);
    hpw::set_gamma(1.0);
    graphic::set_disable_frame_limit(false);
    graphic::show_fps = false;
    disable_pge();
    set_default();
  } );
}

Shared<Menu_text_item> Scene_graphic::get_gamma_item() {
  return new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.gamma.title"),
    []{ hpw::scene_mgr->add(new_shared<Scene_gamma>()); } );
}

void Scene_graphic::init_simple_menu() {
  init_shared<Advanced_text_menu>( simple_menu,
    get_locale_str("scene.graphic_menu.title"),
    Menu_items {
      get_palette_item(),
      get_plugin_item(),
      new_shared<Menu_text_item>(
        get_locale_str("scene.graphic_menu.pressets.name"),
        [this]{ cur_menu = preset_menu; }
      ),
      get_fullscreen_item(),
      get_resize_type_item(),
      get_vsync_item(),
      get_frame_limit_item(),
      get_disable_frame_limit_item(),
      get_goto_detailed_item(),
      get_epilepsy_item(),
      get_reset_item(),
      get_exit_item(),
    },
    Rect{0, 0, graphic::width, graphic::height}
  );
  // звук при выборе пункта меню
  simple_menu->set_select_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/close.flac"); } );
  // звук при перемещении по пунктам меню
  simple_menu->set_move_cursor_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/select 2.flac"); } );
} // init_simple_menu

void Scene_graphic::init_detailed_menu() {
  init_shared<Advanced_text_menu>( detailed_menu,
    U"Расширенные настройки графики", // TODO locale
    Menu_items {
      get_palette_item(),
      get_plugin_item(),
      get_epilepsy_item(),
      get_fullscreen_item(),
      get_vsync_item(),
      get_gamma_item(),
      get_draw_border_item(),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.double_buffering"),
        []{ return graphic::double_buffering; },
        [](bool new_val) { hpw::set_double_buffering(new_val); },
        get_locale_str("scene.graphic_menu.description.double_buffering")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.enable_motion_blur"),
        []{ return graphic::enable_motion_blur; },
        [](bool new_val) { graphic::enable_motion_blur = new_val; },
        get_locale_str("scene.graphic_menu.description.enable_motion_blur")
      ),
      get_mouse_cursour_item(),
      get_disable_frame_limit_item(),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.wait_frame"),
        []{ return graphic::wait_frame; },
        [](bool new_val) { graphic::wait_frame_bak = graphic::wait_frame = new_val; },
        get_locale_str("scene.graphic_menu.description.wait_frame")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.cpu_safe"),
        []{ return graphic::cpu_safe; },
        [](bool new_val) { graphic::cpu_safe = new_val; },
        get_locale_str("scene.graphic_menu.description.cpu_safe")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.blink_particles"),
        []{ return graphic::blink_particles; },
        [](bool new_val) { graphic::blink_particles = new_val; },
        get_locale_str("scene.graphic_menu.description.blink_particles")
      ),
      get_frame_limit_item(),
      get_resize_type_item(),
      new_shared<Menu_double_item>(
        get_locale_str("scene.graphic_menu.blur_quality_mul"),
        [] { return graphic::blur_quality_mul; },
        [] (double val) { graphic::blur_quality_mul = std::clamp(val, 0.5, 8.0); },
        0.25, // step speed
        get_locale_str("scene.graphic_menu.description.blur_quality_mul")
      ),
      new_shared<Menu_int_item>(
        get_locale_str("scene.graphic_menu.frame_skip"),
        []{ return graphic::frame_skip; },
        [](int val){ graphic::frame_skip = std::clamp(val, 0, 30); },
        1,
        get_locale_str("scene.graphic_menu.description.frame_skip")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.auto_frame_skip"),
        []{ return graphic::auto_frame_skip; },
        [](bool val){ graphic::auto_frame_skip = val; },
        get_locale_str("scene.graphic_menu.description.auto_frame_skip")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.enable_motion_interp"),
        []{ return graphic::enable_motion_interp; },
        [](bool val) {
          if (!val)
            graphic::enable_motion_blur = false;
          graphic::enable_motion_interp = val;
        },
        get_locale_str("scene.graphic_menu.description.enable_motion_interp")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.show_fps.title"),
        [] { return graphic::show_fps; },
        [] (bool new_val) { graphic::show_fps = new_val; },
        get_locale_str("scene.graphic_menu.show_fps.desc")
      ),
      new_shared<Menu_bool_item>(
        get_locale_str("scene.graphic_menu.fast_blur.title"),
        [] { return graphic::fast_blur; },
        [] (bool new_val) { graphic::fast_blur = new_val; },
        get_locale_str("scene.graphic_menu.fast_blur.desc")
      ),
      get_reset_item(),
      new_shared<Menu_text_item>(
        get_locale_str("common.back"),
        [this]{ cur_menu = simple_menu; }
      ),
      get_exit_item(),
    },
    Rect{0, 0, graphic::width, graphic::height}
  );
  // звук при выборе пункта меню
  detailed_menu->set_select_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/close.flac"); } );
  // звук при перемещении по пунктам меню
  detailed_menu->set_move_cursor_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/select 2.flac"); } );
} // init_detailed_menu

void Scene_graphic::init_preset_menu() {
  init_shared<Advanced_text_menu>( preset_menu,
    get_locale_str("scene.graphic_menu.pressets.name"),
    Menu_items {
      get_preset_item(),
      new_shared<Menu_text_item>(
        get_locale_str("common.back"),
        [this]{ cur_menu = simple_menu; }
      ),
      get_exit_item(),
    },
    Rect{0, 0, graphic::width, graphic::height}
  );
  // звук при выборе пункта меню
  preset_menu->set_select_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/close.flac"); } );
  // звук при перемещении по пунктам меню
  preset_menu->set_move_cursor_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/select 2.flac"); } );
}
