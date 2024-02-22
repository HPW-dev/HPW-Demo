#include <cassert>
#include <algorithm>
#include "game-app.hpp"
#include "host/command.hpp"
#include "game/util/pge.hpp"
#include "game/util/game-util.hpp"
#include "game/core/scenes.hpp"
#include "game/scene/scene-main-menu.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/core/core.hpp"
#include "game/core/fonts.hpp"
#include "game/core/canvas.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"
#include "game/core/locales.hpp"
#include "game/util/config.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-archive.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/image/color-table.hpp"
#include "util/file/archive.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"

Game_app::Game_app(int argc, char *argv[])
: Host_glfw(argc, argv)
{
  #ifdef RELEASE
    init_validation_info();
  #endif
  check_color_tables();
  load_resources();
  load_locale();
  load_font();
  
  init_scene_mgr();
  hpw::scene_mgr->add( new_shared<Scene_main_menu>() );

  /* к этому моменту кеймапер будет инициализирован и
  управление можно будет переназначить с конфига */
  load_config();
  load_pge_from_config();
} // c-tor

Game_app::~Game_app() {
  disable_pge();
}

void Game_app::update(double dt) {
  ALLOW_STABLE_RAND
  assert(dt == hpw::target_update_time);
  update_graphic_autoopt(dt);
  
  Host_glfw::update(dt);

  auto st = get_time();
  if ( !hpw::scene_mgr->update(dt) ) {
    detailed_log("scenes are over, call soft_exit\n");
    hpw::soft_exit();
  }
  hpw::update_time_unsafe = get_time() - st;
} // update

void Game_app::draw_game_frame() {
  auto st = get_time();

  hpw::scene_mgr->draw(*graphic::canvas);
  if (graphic::draw_border) // рамка по краям
    draw_border(*graphic::canvas);
  apply_pge(graphic::frame_count);

  graphic::soft_draw_time = get_time() - st;
  graphic::check_autoopt();
}

void Game_app::draw_border(Image& dst) const
  { draw_rect(dst, Rect{0,0, dst.X, dst.Y}, Pal8::white); }

void Game_app::load_locale() {
  auto path = (*hpw::config)["path"].get_str("locale", "resource/locale/en.yml");
  auto mem = hpw::archive->get_file(path);
  auto yml = Yaml(mem);
  load_locales_to_store(yml);
}

void Game_app::load_font() {
  auto mem {hpw::archive->get_file("resource/font/unifont-13.0.06.ttf")};
  graphic::font = new_shared<Unifont>(mem, 16, true);
}

void Game_app::update_graphic_autoopt(double dt) {
  using timeout_t = decltype(graphic::autoopt_timeout);
  // если рендер не будет лагать, то после таймера - тригер автооптимизации сбросится
  graphic::autoopt_timeout = std::max(graphic::autoopt_timeout - dt, timeout_t(0));
  if (graphic::autoopt_timeout == timeout_t(0))
    graphic::render_lag = false;
}
