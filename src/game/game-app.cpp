#include <cassert>
#include <algorithm>
#include "game-app.hpp"
#include "host/command.hpp"
#include "game/scene/scene-main-menu.hpp"
#include "game/scene/scene-locale.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/scene/scene-game.hpp"
#include "game/scene/scene-graphic.hpp"
#include "game/scene/scene-nickname.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "game/core/scenes.hpp"
#include "game/core/core.hpp"
#include "game/core/epges.hpp"
#include "game/core/replays.hpp"
#include "game/core/tasks.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/debug.hpp"
#include "game/core/sounds.hpp"
#include "game/core/common.hpp"
#include "game/util/pge.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/dbg-plots.hpp"
#include "game/util/locale.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/font-helper.hpp"
#include "game/util/sound-helper.hpp"
#include "game/util/validation.hpp"
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
  load_fonts();

  /* к этому моменту кеймапер будет инициализирован и
  управление можно будет переназначить с конфига */
  load_config();
  load_pge_from_config();
  load_sounds();

  // управление сценами
  hpw_log("настройка игровых сцен...\n");
  #ifndef DEBUG
  hpw::empty_level_first = false;
  #endif
  if (hpw::empty_level_first) {
    hpw_log("start debug level...\n");
    hpw::scene_mgr.add( new_shared<Scene_game>() );
  } else {
    hpw::scene_mgr.add( new_shared<Scene_main_menu>() );
    // спросить о языке при первом запуске
    if (hpw::first_start) {
      // набрать никнейм
      hpw::scene_mgr.add(new_shared<Scene_nickname>());
      // выбрать язык
      hpw::scene_mgr.add( new_shared<Scene_locale_select>() );
    }
  }

  if (hpw::first_start) {
    #ifndef DEBUG
    // поставить хороший графон в релизной версии
    set_high_quality();
    #endif
    hpw::set_resize_mode(graphic::default_resize_mode);
  }
  hpw::first_start = false;

  hpw_log("загрузка завершена\n");
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

  if (hpw::replay_read_mode)
    replay_load_keys();
  elif (hpw::enable_replay)
    replay_save_keys();

  if ( !hpw::scene_mgr.update(dt) ) {
    hpw_log("scenes are over, call soft_exit\n", Log_stream::debug);
    hpw::soft_exit();
  }

  check_errors();
  hpw::tick_time = get_time() - st;
}

void Game_app::draw_game_frame() const {
  cauto st = get_time();
  assert(graphic::canvas);
  auto& dst = *graphic::canvas;

  hpw::scene_mgr.draw(dst);
  post_draw(dst);

  graphic::soft_draw_time = get_time() - st;
  graphic::check_autoopt();

  Host_class::draw_game_frame(); // hardware draw
}

void Game_app::draw_border(Image& dst) const
  { draw_rect(dst, Rect{0,0, dst.X, dst.Y}, Pal8::white); }

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
    hpw::scene_mgr.add(new_shared<Scene_msgbox_enter>(
      get_locale_str("sound_settings.device_init_error"),
      get_locale_str("common.error")
    ));
  }

  // ошибка при мультиоконном запуске
  if (hpw::multiple_apps) {
    hpw::multiple_apps = false;
    hpw::scene_mgr.add(new_shared<Scene_msgbox_enter>(
      get_locale_str("common.multiapp_warning"),
      get_locale_str("common.warning")
    ));
  }

  // пользовательские сообщения
  if (!hpw::user_warnings.empty()) {
    hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(hpw::user_warnings, get_locale_str("common.warning")) );
    hpw::user_warnings.clear();
  }
} // check_errors

void Game_app::replay_save_keys() {
  return_if(!hpw::replay);
  Key_packet packet;

  #define check_key(key) if (is_pressed(key)) \
    packet.emplace_back(key);
  check_key(hpw::keycode::up)
  check_key(hpw::keycode::down)
  check_key(hpw::keycode::left)
  check_key(hpw::keycode::right)
  check_key(hpw::keycode::escape)
  check_key(hpw::keycode::enable)
  check_key(hpw::keycode::focus)
  check_key(hpw::keycode::mode)
  check_key(hpw::keycode::bomb)
  check_key(hpw::keycode::shoot)
  check_key(hpw::keycode::fulscrn)
  check_key(hpw::keycode::fps)
  check_key(hpw::keycode::text_delete)
  #ifdef DEBUG
  check_key(hpw::keycode::reset)
  check_key(hpw::keycode::debug)
  check_key(hpw::keycode::console)
  check_key(hpw::keycode::fast_forward)
  #endif
  #undef check_key

  hpw::replay->push( std::move(packet) );
}

void Game_app::replay_load_keys() {
  return_if(!hpw::replay);
  // сбросить свои клавиши
  clear_cur_keys();
  hpw::any_key_pressed = false;

  // прочитать клавиши с реплея
  auto key_packet = hpw::replay->pop();
  if (key_packet) {
    for (crauto key: *key_packet) {
      press(key);
      hpw::any_key_pressed = true;
    }
  } else {
    // по завершению реплея выходить из сцены обратно
    hpw_log("replay end\n");
    hpw::replay_read_mode = false;
    hpw::replay = {};
    //hpw::scene_mgr.back(); TODO?
  }
}

void Game_app::post_draw(Image& dst) const {
  if (graphic::show_fps) // отобразить фпс
    draw_fps_info(dst);
  
  if (graphic::draw_border) // рамка по краям
    draw_border(dst);

  hpw::global_task_mgr.draw(dst);

  // EPGE effects:
  for (crauto epge: graphic::epges) {
    epge->update(hpw::real_dt);
    epge->draw(dst);
  }

  // .DLL/.SO effects:
  apply_pge(graphic::frame_count);
}
