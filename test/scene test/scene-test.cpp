#include "scene-test.hpp"
#include "game/util/game-util.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/scene/scene-info.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/unifont.hpp"
#include "util/file/archive.hpp"

void Scene_test::init(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  init_archive();
  auto font_file {hpw::archive->get_file(
    "resource/font/unifont-13.0.06.ttf")};
  init_shared<Unifont>(graphic::font, font_file, 16);
  init_scene_mgr();
  hpw::scene_mgr->add(new_shared<Scene_info>());
} // init

void Scene_test::update(const Delta_time dt) { hpw::scene_mgr->update(dt); }

void Scene_test::draw() const { hpw::scene_mgr->draw(*graphic::canvas); }
