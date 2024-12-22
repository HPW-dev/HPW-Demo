#include <cassert>
#include "config.hpp"
#include "game/core/core.hpp"
#include "game/core/huds.hpp"
#include "game/core/canvas.hpp"
#include "game/core/user.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/locales.hpp"
#include "game/core/common.hpp"
#include "game/core/debug.hpp"
#include "game/core/palette.hpp"
#include "game/core/replays.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "util/file/yaml.hpp"
#include "util/file/file-io.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/safecall.hpp"
#include "host/host-util.hpp"
#include "host/command.hpp"
#include "plugin/epge/epge-util.hpp"

static inline void load_log_config(cr<Yaml> config) {
  auto cfg = log_get_config();
  cfg.print_source = config.get_bool("print_source", cfg.print_source);
  hpw::log_file_path = config.get_str("file_name", hpw::log_file_path);
  
  auto output_node = config["output"];
  cfg.to_file = output_node.get_bool("file", cfg.to_file);
  cfg.to_stdout = output_node.get_bool("stdout", cfg.to_stdout);
  cfg.to_stderr = output_node.get_bool("stderr", cfg.to_stderr);

  auto streams_node = config["streams_enabled"];
  cfg.stream_debug = streams_node.get_bool("debug", cfg.stream_debug);
  cfg.stream_info = streams_node.get_bool("info", cfg.stream_info);
  cfg.stream_warning = streams_node.get_bool("warning", cfg.stream_warning);

  log_set_config(cfg);
}

static inline void save_log_config(Yaml& config) {
  cauto cfg = log_get_config();
  config.set_bool("print_source", cfg.print_source);
  config.set_str("file_name", hpw::log_file_path);
  
  auto output_node = config.make_node("output");
  output_node.set_bool("file", cfg.to_file);
  output_node.set_bool("stdout", cfg.to_stdout);
  output_node.set_bool("stderr", cfg.to_stderr);

  auto streams_node = config.make_node("streams_enabled");
  streams_node.set_bool("debug", cfg.stream_debug);
  streams_node.set_bool("info", cfg.stream_info);
  streams_node.set_bool("warning", cfg.stream_warning);
}

int get_scancode(const hpw::keycode keycode) {
  if(hpw::keys_info.keys.empty())
    return -1;
  cauto key_info = hpw::keys_info.find(keycode);
  assert(key_info);
  return key_info->scancode;
}

static inline void load_test_image_path(cr<Yaml> config)
  { graphic::cur_test_image_path = config.get_str("test_image_path", graphic::cur_test_image_path); }

static inline void save_test_image_path(Yaml& config)
  { config.set_str("test_image_path", graphic::cur_test_image_path); }
  
static inline void load_light_quality(cr<Yaml> config) {
  graphic::light_quality = scast<Light_quality>(
    config.get_int("light_quality", scast<int>(graphic::light_quality)) );
}

static inline void save_light_quality(Yaml& config)
  { config.set_int("light_quality", scast<int>(graphic::light_quality)); }

static inline void load_heat_distort_mode(cr<Yaml> config) {
  graphic::heat_distort_mode = scast<Heat_distort_mode>(
    config.get_int("heat_distort_mode", scast<int>(graphic::heat_distort_mode)) );
}

static inline void save_heat_distort_mode(Yaml& config)
  { config.set_int("heat_distort_mode", scast<int>(graphic::heat_distort_mode)); }

static inline void setup_log() {
  auto cfg = log_get_config();
  // создавать лог-файлы по умолчанию
  if (hpw::first_start) {
    cfg.to_file = true;
    #ifdef RELEASE
    cfg.to_stderr = false;
    cfg.to_stdout = false;
    #endif
    log_set_config(cfg);
  }
  if (cfg.to_file)
    log_open_file(Str(hpw::cur_dir + hpw::log_file_path).c_str());
}

static inline void save_nickname() {
  static_assert(std::is_same_v<utf32, decltype(hpw::player_name)>);
  static_assert(sizeof(char32_t) == sizeof(decltype(hpw::player_name)::value_type));

  std::uint32_t nick_sz = hpw::player_name.size() * sizeof(char32_t);
  nick_sz = std::min(nick_sz, hpw::MAX_NICKNAME_SZ);

  if (nick_sz) {
    File_writer fw(hpw::cur_dir + hpw::nickname_path);
    fw.write(cptr2ptr<cp<byte>>(hpw::player_name.data()), nick_sz);
  }
}

static inline void load_nickname() {
  static_assert(std::is_same_v<utf32, decltype(hpw::player_name)>);
  static_assert(sizeof(char32_t) == sizeof(decltype(hpw::player_name)::value_type));

  try {
    File_reader fr(hpw::cur_dir + hpw::nickname_path);
    std::uint32_t nick_sz = fr.size();
    nick_sz = std::min(nick_sz, hpw::MAX_NICKNAME_SZ);

    if (nick_sz == 0) {
      hpw::player_name = {};  
      return;
    }

    hpw::player_name.resize(nick_sz / sizeof(char32_t));
    assert(hpw::player_name.size() == nick_sz / sizeof(char32_t));
    fr.read(ptr2ptr<byte*>(hpw::player_name.data()), nick_sz);
  } catch (...) {
    hpw_warning("не удалось загрузить файл с никнеймом игрока. Ник будет пустым\n");
    hpw::player_name = {};
  }
}

void save_config() {
  auto& config = *hpw::config;

  config.set_bool("first_start", hpw::first_start);
  config.set_bool("enable_replay", hpw::enable_replay);
  config.set_bool("need_tutorial", hpw::need_tutorial);

  auto game_node = config.make_node("game");
  game_node.set_bool("rnd_pal_after_death", hpw::rnd_pal_after_death);
  game_node.set_bool("collider_autoopt", hpw::collider_autoopt);
  game_node.set_str ("locale", hpw::locale_path);
  game_node.set_str ("hud", graphic::cur_hud);
  save_nickname();

  auto path_node = config.make_node("path");
  path_node.set_str("screenshots", hpw::screenshots_path);
  path_node.set_str("data", hpw::data_path);
  path_node.set_str("os_resources_dir", hpw::os_resources_dir);
  path_node.set_str("replays_dir", hpw::replays_path);

  auto debug = config.make_node("debug");
  debug.set_bool("empty_level_first", hpw::empty_level_first);
  debug.set_str ("start_script", hpw::start_script);

  auto graphic_node = config.make_node("graphic");
  graphic_node.set_v_int("canvas_size",        {graphic::width, graphic::height} );
  graphic_node.set_int  ("light_quality",       scast<int>(graphic::light_quality) );
  graphic_node.set_bool ("enable_motion_interp",graphic::enable_motion_interp);
  graphic_node.set_bool ("fullscren",           graphic::fullscreen);
  graphic_node.set_bool ("draw_border",         graphic::draw_border);
  graphic_node.set_bool ("show_mouse_cursour",  graphic::show_mouse_cursour);
  graphic_node.set_int  ("resize_mode",         scast<int>(graphic::resize_mode) );
  graphic_node.set_bool ("motion_blur_mode",    scast<int>(graphic::motion_blur_mode) );
  graphic_node.set_bool ("blur_mode",           scast<int>(graphic::blur_mode) );
  graphic_node.set_real ("motion_blur_quality_mul", graphic::motion_blur_quality_mul);
  graphic_node.set_bool ("blink_particles",     graphic::blink_particles);
  graphic_node.set_real ("max_motion_blur_quality_reduct", graphic::max_motion_blur_quality_reduct);
  graphic_node.set_bool ("start_focused",       graphic::start_focused);
  graphic_node.set_str  ("palette",             graphic::current_palette_file);
  graphic_node.set_int  ("frame_skip",          graphic::frame_skip);
  graphic_node.set_bool ("auto_frame_skip",     graphic::auto_frame_skip);
  graphic_node.set_real ("gamma",               graphic::gamma);
  graphic_node.set_bool ("show_fps",            graphic::show_fps);
  save_light_quality(graphic_node);
  save_heat_distort_mode(graphic_node);
  save_test_image_path(graphic_node);

  auto epge_node = graphic_node.make_node("epge");
  save_epges(epge_node);

  auto log_node = config.make_node("log");
  save_log_config(log_node);

  auto sync_node = graphic_node.make_node("sync");
  sync_node.set_bool("vsync",               graphic::get_vsync());
  sync_node.set_bool("wait_frame",          graphic::wait_frame);
  sync_node.set_int ("target_fps",          graphic::get_target_fps());
  sync_node.set_bool("cpu_safe",            graphic::cpu_safe);
  sync_node.set_real("autoopt_timeout_max", graphic::autoopt_timeout_max);
  sync_node.set_bool("disable_frame_limit", graphic::get_disable_frame_limit());

  auto input_node = config.make_node("input");
  #define SAVE_KEY(name) input_node.set_int(#name, get_scancode(hpw::keycode::name));
  SAVE_KEY(enable)
  SAVE_KEY(escape)
  SAVE_KEY(bomb)
  SAVE_KEY(shoot)
  SAVE_KEY(focus)
  SAVE_KEY(mode)
  SAVE_KEY(up)
  SAVE_KEY(down)
  SAVE_KEY(left)
  SAVE_KEY(right)
  SAVE_KEY(screenshot)
  SAVE_KEY(fulscrn)
  #undef SAVE_KEY

  make_dir_if_not_exist(hpw::cur_dir + hpw::config_dir);
  config.save( hpw::config->get_path() );
}

void load_config() {
  hpw_log("чтение конфига...\n");
  make_dir_if_not_exist(hpw::cur_dir + hpw::config_dir);
  init_shared(hpw::config, hpw::cur_dir + hpw::config_path, true);
  hpw_log("файл конфига: \"" + hpw::cur_dir + hpw::config_path + "\"\n");

  crauto config = *hpw::config;
  hpw::first_start = config.get_bool("first_start", true);
  hpw::enable_replay = config.get_bool("enable_replay", hpw::enable_replay);
  hpw::need_tutorial = config.get_bool("need_tutorial", hpw::need_tutorial);
  
  cauto debug = config["debug"];
  hpw::empty_level_first = debug.get_bool("empty_level_first", hpw::empty_level_first);
  hpw::start_script = debug.get_str("start_script", hpw::start_script);

  cauto path_node = config["path"];
  hpw::screenshots_path = path_node.get_str("screenshots", hpw::screenshots_path);
  hpw::data_path = path_node.get_str("data", hpw::data_path);
  hpw::os_resources_dir = path_node.get_str("os_resources_dir", hpw::os_resources_dir);
  hpw::replays_path = path_node.get_str("replays_dir", hpw::replays_path);

  // сделать папки, если их нет
  make_dir_if_not_exist(hpw::cur_dir + path_node.get_str("screenshots"));
  make_dir_if_not_exist(hpw::cur_dir + hpw::replays_path);

  cauto graphic_node = config["graphic"];
  load_config_graphic(graphic_node);

  cauto log_node = config["log"];
  load_log_config(log_node);

  cauto game_node = config["game"];
  load_config_game(game_node);
  load_nickname();

  cauto input_node = config["input"];
  load_config_input(input_node);

  setup_log();
}

static inline void node_check(cr<Yaml> config) {
  if (!config.check())
    hpw_log("не удалось загрузить конфиг: \"" + config.get_path() +
      "\". Будут загружены значения по умолчанию\n", Log_stream::warning);
}

void load_config_input(cr<Yaml> config) {
  node_check(config);
  if (hpw::rebind_key_by_scancode) {
    #define LOAD_KEY(name) hpw::rebind_key_by_scancode(hpw::keycode::name, config.get_int(#name, get_scancode(hpw::keycode::name)) );
    LOAD_KEY(enable)
    LOAD_KEY(escape)
    LOAD_KEY(bomb)
    LOAD_KEY(shoot)
    LOAD_KEY(focus)
    LOAD_KEY(mode)
    LOAD_KEY(up)
    LOAD_KEY(down)
    LOAD_KEY(left)
    LOAD_KEY(right)
    LOAD_KEY(screenshot)
    LOAD_KEY(fulscrn)
    #undef LOAD_KEY
  }
}

void load_config_graphic(cr<Yaml> config) {
  node_check(config);
  cauto canvas_size = config.get_v_int("canvas_size", {graphic::width, graphic::height});
  graphic::width  = canvas_size.at(0);
  graphic::height = canvas_size.at(1);
  graphic::enable_motion_interp = config.get_bool("enable_motion_interp", graphic::enable_motion_interp);
  graphic::fullscreen = config.get_bool("fullscren", graphic::fullscreen);
  graphic::draw_border = config.get_bool("draw_border", graphic::draw_border);
  graphic::show_mouse_cursour = config.get_bool("show_mouse_cursour", graphic::show_mouse_cursour);
  graphic::resize_mode = scast<decltype(graphic::resize_mode)> (
    config.get_int("resize_mode", scast<int>(graphic::default_resize_mode)) );
  graphic::light_quality = scast<decltype(graphic::light_quality)>(
    config.get_int("light_quality", scast<int>(graphic::light_quality)) );
  graphic::motion_blur_mode = scast<Motion_blur_mode>(
    config.get_int("motion_blur_mode", scast<int>(graphic::motion_blur_mode)) );
  graphic::blur_mode = scast<Blur_mode>(
    config.get_int("blur_mode", scast<int>(graphic::blur_mode)) );
  graphic::motion_blur_quality_mul = config.get_real("motion_blur_quality_mul", graphic::motion_blur_quality_mul);
  graphic::blink_particles = config.get_bool("blink_particles", graphic::blink_particles);
  graphic::max_motion_blur_quality_reduct =
    config.get_real("max_motion_blur_quality_reduct", graphic::max_motion_blur_quality_reduct);
  graphic::start_focused = config.get_bool("start_focused", graphic::start_focused);
  safecall(hpw::init_palette_from_archive, config.get_str("palette", graphic::current_palette_file));
  graphic::frame_skip = config.get_int("frame_skip", graphic::frame_skip);
  graphic::auto_frame_skip = config.get_bool("auto_frame_skip", graphic::auto_frame_skip);
  safecall(hpw::set_gamma, config.get_real("gamma", graphic::gamma));
  graphic::show_fps = config.get_bool("show_fps", graphic::show_fps);
  load_light_quality(config);
  load_heat_distort_mode(config);
  load_test_image_path(config);

  cauto sync_node = config["sync"];
  node_check(sync_node);
  graphic::set_vsync( sync_node.get_bool("vsync", graphic::get_vsync()) );
  graphic::wait_frame_bak = graphic::wait_frame = sync_node.get_bool("wait_frame", graphic::wait_frame);
  graphic::set_disable_frame_limit( sync_node.get_bool("disable_frame_limit", graphic::get_disable_frame_limit()) );
  graphic::set_target_fps( sync_node.get_int("target_fps", graphic::get_target_vsync_fps()) );
  graphic::cpu_safe = sync_node.get_bool("cpu_safe", graphic::cpu_safe);
  graphic::autoopt_timeout_max = sync_node.get_real("autoopt_timeout_max", graphic::autoopt_timeout_max);

  cauto epge_node = config["epge"];
  load_epges(epge_node);
}

void load_config_game(cr<Yaml> config) {
  node_check(config);
  hpw::rnd_pal_after_death = config.get_bool("rnd_pal_after_death", hpw::rnd_pal_after_death);
  hpw::collider_autoopt = config.get_bool("collider_autoopt", hpw::collider_autoopt);
  graphic::cur_hud = config.get_str("hud", graphic::cur_hud);

  try {
    hpw::locale_path = config.get_str("locale", hpw::locale_path);
    load_locale(hpw::locale_path);
  } catch (...) {
    hpw_log("не удалось загрузить файл локализации \"" + hpw::locale_path + "\"\n", Log_stream::debug);
  }
}
