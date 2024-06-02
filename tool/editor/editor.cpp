#include <cassert>
#include "editor.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/util/game-util.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/core/core.hpp"
#include "game/core/graphic.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "graphic/image/image.hpp"
#include "host/command.hpp"
#include "scene/scene-editor.hpp"

Editor::Editor(int argc, char *argv[])
: Host_imgui(argc, argv) { }

void Editor::init() {
  Host_imgui::init();
  load_config(); // refresh config
  init_scene_mgr();
  init_graphic();
  hpw::scene_mgr->add(new_shared<Editor_scene>());
  // при старте редактор сам растягивается и перетаскивается на нужное место
  set_window_pos(200, 25);
  reshape(1024, 720);
}

void Editor::update(const Delta_time dt) {
  Host_imgui::update(dt);
  auto st = Editor::get_time();
  if ( !hpw::scene_mgr->update(dt) ) {
    detailed_log("scenes are over, call soft_exit\n");
    hpw::soft_exit();
  }
  hpw::update_time_unsafe = Editor::get_time() - st;
}

void Editor::draw() {
  Host_imgui::draw();
  auto st = Editor::get_time();
  hpw::scene_mgr->draw(*graphic::canvas);
  graphic::soft_draw_time = Editor::get_time() - st;
}

void Editor::imgui_exec() {
  auto scene {hpw::scene_mgr->get_current()};
  return_if ( !scene);
  auto editor_scene {dcast<Editor_scene_base*>(scene)};
  assert(editor_scene);
  editor_scene->imgui_exec();
}

void Editor::init_graphic() {
  hpw::set_double_buffering(true);
  graphic::wait_frame = false;
  graphic::cpu_safe = false;
  hpw::set_vsync(false);
  graphic::set_disable_frame_limit(false);
  graphic::set_target_fps(60);
  hpw::set_fullscreen(false);
  hpw::set_resize_mode(graphic::default_resize_mode);
  graphic::enable_heat_distort = true;
  graphic::enable_motion_blur = true;
  graphic::enable_light = true;
}
