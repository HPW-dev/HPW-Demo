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
#include "game/scene/scene-cmd.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "game/core/messages.hpp"
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

#ifdef DEBUG
#include "game/scene/scene-cmd.hpp"
#include "game/util/cmd/cmd-script.hpp"
#endif

void Game_app::startup_script() {
  if (!hpw::start_script.empty())
    hpw::cmd.exec("script " + hpw::cur_dir + hpw::start_script);
}

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

  init_unique(hpw::message_mgr);

  // управление сценами
  log_info << "настройка игровых сцен...";
  #ifndef DEBUG
  hpw::empty_level_first = false;
  #endif
  if (hpw::empty_level_first) {
    log_info << "start debug level...";
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

  startup_script();

  if (hpw::first_start) {
    #ifndef DEBUG
    // поставить хороший графон в релизной версии
    set_high_quality();
    #endif
    hpw::set_resize_mode(graphic::default_resize_mode);
  }
  hpw::first_start = false;

  log_info << "загрузка завершена";
} // c-tor

Game_app::~Game_app() {
  disable_pge();
  hpw::scene_mgr.clear();
}

void Game_app::update(const Delta_time dt) {
  hpw::tick_start_time_prev = hpw::tick_start_time;
  hpw::tick_start_time = get_time();

  ALLOW_STABLE_RAND
  assert(dt == hpw::target_tick_time);
  update_graphic_autoopt(dt);

  Host_class::update(dt);

  if (hpw::replay_read_mode)
    replay_load_keys();
  elif (hpw::enable_replay)
    replay_save_keys();

  if ( !hpw::scene_mgr.update(dt) ) {
    log_debug << "scenes are over, call soft_exit";
    hpw::soft_exit();
  }
  
  #ifdef DEBUG
  // консоль команд
  if (is_pressed_once(hpw::keycode::console)) {
    hpw::scene_mgr.add(new_shared<Scene_cmd>());
    release(hpw::keycode::console); // клавиша должна отлипнуть, иначе будет вход и выход сразу
  }
  #endif

  assert(hpw::message_mgr);
  hpw::message_mgr->update(dt);

  check_errors();
  
  hpw::tick_end_time = get_time();
  hpw::tick_time = std::max<Delta_time>(0, hpw::tick_end_time - hpw::tick_start_time);
}

void Game_app::draw_game_frame() const {
  cauto st = get_time();
  
  // лимит значения чтобы при тормозах окна объекты не растягивались
  hpw::soft_draw_start_time = st;
  graphic::lerp_alpha = safe_div(hpw::soft_draw_start_time - hpw::tick_end_time, hpw::target_tick_time);
  graphic::lerp_alpha = std::clamp<Delta_time>(graphic::lerp_alpha, 0, 0.9999999);

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
    log_info << "replay end";
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

  hpw::task_mgr.draw(dst);
  assert(hpw::message_mgr);
  hpw::message_mgr->draw(dst);

  // EPGE effects:
  for (crauto epge: graphic::epges) {
    epge->update(hpw::real_dt);
    epge->draw(dst);
  }

  // .DLL/.SO effects:
  apply_pge(graphic::frame_count);
}
