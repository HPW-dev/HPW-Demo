#include <cassert>
#include "editor.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/sound-helper.hpp"
#include "game/core/core.hpp"
#include "game/core/graphic.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "host/command.hpp"
#include "util/log.hpp"

//#define ENTITY_EDITOR_DEBUG
#ifdef ENTITY_EDITOR_DEBUG
#include "scene/scene-entity-editor.hpp"
#else
#include "scene/scene-editor-select.hpp"
#endif

Editor::Editor(int argc, char *argv[])
: Host_imgui(argc, argv) { }

void Editor::init() {
  Host_imgui::init();
  load_config(); // refresh config
  load_sounds();
  init_graphic();
  #ifdef ENTITY_EDITOR_DEBUG
    hpw::scene_mgr.add(new_shared<Scene_entity_editor>());
  #else
    hpw::scene_mgr.add(new_shared<Scene_editor_select>());
  #endif
  // при старте редактор сам растягивается и перетаскивается на нужное место
  set_window_pos(200, 25);
  reshape(1024, 720);
}

inline static void update_graphic_autoopt(const Delta_time dt) {
  using timeout_t = decltype(graphic::autoopt_timeout);
  // если рендер не будет лагать, то после таймера - тригер автооптимизации сбросится
  graphic::autoopt_timeout = std::max(graphic::autoopt_timeout - dt, timeout_t(0));
  if (graphic::autoopt_timeout == timeout_t(0))
    graphic::render_lag = false;
}

void Editor::update(const Delta_time dt) {
  update_graphic_autoopt(dt);
  Host_imgui::update(dt);
  auto st = Editor::get_time();
  if ( !hpw::scene_mgr.update(dt) ) {
    log_debug("scenes are over, call soft_exit\n");
    hpw::soft_exit();
  }
  hpw::tick_time = Editor::get_time() - st;
}

void Editor::draw_game_frame() const {
  Host_imgui::draw_game_frame();
  auto st = Editor::get_time();
  hpw::scene_mgr.draw(*graphic::canvas);
  graphic::soft_draw_time = Editor::get_time() - st;
  graphic::check_autoopt();
}

void Editor::imgui_exec() const {
  auto scene {hpw::scene_mgr.current()};
  return_if (!scene);

  auto editor_scene {dcast<Editor_scene_base*>(scene.get())};
  return_if (!editor_scene);
  
  editor_scene->imgui_exec();
}

void Editor::init_graphic() {
  hpw::set_double_buffering(true);
  graphic::wait_frame_bak = graphic::wait_frame = false;
  graphic::cpu_safe = false;
  hpw::set_vsync(false);
  graphic::set_disable_frame_limit(false);
  graphic::set_target_fps(60);
  hpw::set_fullscreen(false);
  hpw::set_resize_mode(graphic::default_resize_mode);
  graphic::heat_distort_mode = Heat_distort_mode::autoopt;
  graphic::motion_blur_mode = Motion_blur_mode::autoopt;
  graphic::light_quality = Light_quality::medium;
}
