#include <cassert>
#include <algorithm>
#include "game-app.hpp"
#include "host/command.hpp"
#include "game/scene/scene-main-menu.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/scene/scene-game.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "game/core/scenes.hpp"
#include "game/core/core.hpp"
#include "game/core/fonts.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"
#include "game/core/locales.hpp"
#include "game/core/sounds.hpp"
#include "game/core/common.hpp"
#include "game/util/pge.hpp"
#include "game/util/game-util.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/game-util.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-archive.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/image/color-table.hpp"
#include "sound/sound-mgr.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"

Game_app::Game_app(int argc, char *argv[]): Host_class(argc, argv) {
  #ifdef RELEASE
    init_validation_info();
  #endif
  #ifndef ECOMEM
    check_color_tables();
  #endif
  load_resources();
  load_locale();
  load_font();

  /* к этому моменту кеймапер будет инициализирован и
  управление можно будет переназначить с конфига */
  load_config();
  load_pge_from_config();
  load_sounds();

  // управление сценами
  init_scene_mgr();
  #ifndef DEBUG
  hpw::empty_level_first = false;
  #endif
  if (hpw::empty_level_first) {
    hpw_log("start debug level...\n");
    hpw::scene_mgr->add( new_shared<Scene_game>() );
  } else {
    hpw::scene_mgr->add( new_shared<Scene_main_menu>() );
  }
} // c-tor

Game_app::~Game_app() {
  disable_pge();
  hpw::scene_mgr = {};
}

void Game_app::update(const Delta_time dt) {
  ALLOW_STABLE_RAND
  assert(dt == hpw::target_update_time);
  update_graphic_autoopt(dt);
  
  Host_class::update(dt);

  auto st = get_time();
  if ( !hpw::scene_mgr->update(dt) ) {
    detailed_log("scenes are over, call soft_exit\n");
    hpw::soft_exit();
  }
  check_errors();
  hpw::tick_time = get_time() - st;
} // update

void Game_app::draw_game_frame() const {
  cauto st = get_time();

  hpw::scene_mgr->draw(*graphic::canvas);
  if (graphic::draw_border) // рамка по краям
    draw_border(*graphic::canvas);
  apply_pge(graphic::frame_count);
  Host_class::draw_game_frame();

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
  auto mem = hpw::archive->get_file("resource/font/unifont-13.0.06.ttf");
  init_unique<Unifont>(graphic::font, mem, 16, true);
}

void Game_app::update_graphic_autoopt(const Delta_time dt) {
  using timeout_t = decltype(graphic::autoopt_timeout);
  // если рендер не будет лагать, то после таймера - тригер автооптимизации сбросится
  graphic::autoopt_timeout = std::max(graphic::autoopt_timeout - dt, timeout_t(0));
  if (graphic::autoopt_timeout == timeout_t(0))
    graphic::render_lag = false;
}

void Game_app::check_errors() {
  // ошибка при загрузке звуковой системы
  if (hpw::sound_mgr_init_error) {
    hpw::sound_mgr_init_error = false;
    hpw::scene_mgr->add(new_shared<Scene_msgbox_enter>(
      get_locale_str("scene.sound_settings.device_init_error"),
      get_locale_str("common.error")
    ));
  }

  // ошибка при мультиоконном запуске
  if (hpw::multiple_apps) {
    hpw::multiple_apps = false;
    hpw::scene_mgr->add(new_shared<Scene_msgbox_enter>(
      get_locale_str("common.multiapp_warning"),
      get_locale_str("common.warning")
    ));
  }
} // check_errors
