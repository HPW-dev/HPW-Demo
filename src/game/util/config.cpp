#include "pch.hpp"
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
#include "game/core/bgps.hpp"
#include "game/bgp/bgp.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "util/file/file-io.hpp"
#include "host/host-util.hpp"
#include "host/command.hpp"

#ifndef NO_EPGE
#include "engine/graphic/epge/epge-util.hpp"
#endif

static inline void load_log_config(cr<nlohmann::json> config) {
  hpw::log_file_path = config.value<Str>("file_name", hpw::log_file_path);
  hpw::reopen_log_file(hpw::cur_dir + hpw::log_file_path);

  auto streams_node = config["streams"];
  rauto cfg = hpw::logger.config;
  cfg.use_terminal       = streams_node.value<bool>("terminal", cfg.use_terminal);
  cfg.use_stream_info    = streams_node.value<bool>("info",     cfg.use_stream_info);
  cfg.use_stream_warning = streams_node.value<bool>("warning",  cfg.use_stream_warning);
  cfg.use_stream_debug   = streams_node.value<bool>("debug",    cfg.use_stream_debug);
  cfg.use_stream_error   = streams_node.value<bool>("error",    cfg.use_stream_error);
}

static inline void save_log_config(nlohmann::json& config) {
  config.set_str("file_name", hpw::log_file_path);
  
  auto streams_node = make_node(config, "streams");
  streams_node.set_bool("terminal", hpw::logger.config.use_terminal);
  streams_node.set_bool("info", hpw::logger.config.use_stream_info);
  streams_node.set_bool("warning", hpw::logger.config.use_stream_warning);
  streams_node.set_bool("debug", hpw::logger.config.use_stream_debug);
  streams_node.set_bool("error", hpw::logger.config.use_stream_error);
}

int get_scancode(const hpw::keycode keycode) {
  if(hpw::keys_info.keys.empty())
    return -1;
  cauto key_info = hpw::keys_info.find(keycode);
  assert(key_info);
  return key_info->scancode;
}

static inline void load_test_image_path(cr<nlohmann::json> config)
  { graphic::cur_test_image_path = config.value<Str>("test_image_path", graphic::cur_test_image_path); }

static inline void save_test_image_path(nlohmann::json& config)
  { config.set_str("test_image_path", graphic::cur_test_image_path); }
  
static inline void load_light_quality(cr<nlohmann::json> config) {
  graphic::light_quality = scast<Light_quality>(
    config.get_int("light_quality", scast<int>(graphic::light_quality)) );
}

static inline void save_light_quality(nlohmann::json& config)
  { config.set_int("light_quality", scast<int>(graphic::light_quality)); }

static inline void load_heat_distort_mode(cr<nlohmann::json> config) {
  graphic::heat_distort_mode = scast<Heat_distort_mode>(
    config.get_int("heat_distort_mode", scast<int>(graphic::heat_distort_mode)) );
}

static inline void save_heat_distort_mode(nlohmann::json& config)
  { config.set_int("heat_distort_mode", scast<int>(graphic::heat_distort_mode)); }

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
    log_warning << "не удалось загрузить файл с никнеймом игрока. Ник будет пустым";
    hpw::player_name = {};
  }
}

static inline void node_check(cr<nlohmann::json> config) {
  if (!config.check())
    log_warning << "не удалось загрузить конфиг: \"" + config.get_path() +
      "\". Будут загружены значения по умолчанию";
}

static inline void save_game_config(nlohmann::json& config) {
  config.set_bool("rnd_pal_after_death", hpw::rnd_pal_after_death);
  config.set_bool("collider_autoopt", hpw::collider_autoopt);
  config.set_str ("locale", hpw::locale_path);
  config.set_str ("hud", graphic::cur_hud);
  config.set_int ("priority", scast<int>(hpw::process_priority));
  config.set_str ("menu_bgp", hpw::bgp_for_menu);
  config.set_bool("autoswith_bgp", hpw::bgp_auto_swith);
}

void load_config_game(cr<nlohmann::json> config) {
  node_check(config);
  hpw::rnd_pal_after_death = config.value<bool>("rnd_pal_after_death", hpw::rnd_pal_after_death);
  hpw::collider_autoopt = config.value<bool>("collider_autoopt", hpw::collider_autoopt);
  graphic::cur_hud = config.value<Str>("hud", graphic::cur_hud);
  hpw::process_priority = scast<Priority>( config.get_int("priority", scast<int>(hpw::process_priority)) );
  if (hpw::process_priority != Priority::normal)
    set_priority(hpw::process_priority);
  hpw::bgp_for_menu = config.value<Str>("menu_bgp", hpw::bgp_for_menu);
  hpw::bgp_auto_swith = config.value<bool>("autoswith_bgp", hpw::bgp_auto_swith);

  try {
    hpw::locale_path = config.value<Str>("locale", hpw::locale_path);
    load_locale(hpw::locale_path);
  } catch (...) {
    log_error << "не удалось загрузить файл локализации \"" + hpw::locale_path + "\"";
  }
}

void load_config_input(cr<nlohmann::json> config) {
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

void load_config_graphic(cr<nlohmann::json> config) {
  node_check(config);
  cauto canvas_size = config.get_v_int("canvas_size", {graphic::width, graphic::height});
  graphic::width  = canvas_size.at(0);
  graphic::height = canvas_size.at(1);
  graphic::enable_motion_interp = config.value<bool>("enable_motion_interp", graphic::enable_motion_interp);
  graphic::fullscreen = config.value<bool>("fullscren", graphic::fullscreen);
  graphic::draw_border = config.value<bool>("draw_border", graphic::draw_border);
  graphic::show_mouse_cursour = config.value<bool>("show_mouse_cursour", graphic::show_mouse_cursour);
  graphic::resize_mode = scast<decltype(graphic::resize_mode)> (
    config.get_int("resize_mode", scast<int>(graphic::default_resize_mode)) );
  graphic::light_quality = scast<decltype(graphic::light_quality)>(
    config.get_int("light_quality", scast<int>(graphic::light_quality)) );
  graphic::motion_blur_mode = scast<Motion_blur_mode>(
    config.get_int("motion_blur_mode", scast<int>(graphic::motion_blur_mode)) );
  graphic::blur_mode = scast<Blur_mode>(
    config.get_int("blur_mode", scast<int>(graphic::blur_mode)) );
  graphic::motion_blur_quality_mul = config.get_real("motion_blur_quality_mul", graphic::motion_blur_quality_mul);
  graphic::blink_particles = config.value<bool>("blink_particles", graphic::blink_particles);
  graphic::max_motion_blur_quality_reduct =
    config.get_real("max_motion_blur_quality_reduct", graphic::max_motion_blur_quality_reduct);
  graphic::start_focused = config.value<bool>("start_focused", graphic::start_focused);
  safecall(hpw::init_palette_from_archive, config.value<Str>("palette", graphic::current_palette_file));
  graphic::frame_skip = config.get_int("frame_skip", graphic::frame_skip);
  graphic::auto_frame_skip = config.value<bool>("auto_frame_skip", graphic::auto_frame_skip);
  safecall(hpw::set_gamma, config.get_real("gamma", graphic::gamma));
  graphic::show_fps = config.value<bool>("show_fps", graphic::show_fps);
  load_light_quality(config);
  load_heat_distort_mode(config);
  load_test_image_path(config);

  cauto sync_node = config["sync"];
  node_check(sync_node);
  graphic::set_vsync( sync_node.value<bool>("vsync", graphic::get_vsync()) );
  graphic::wait_frame_bak = graphic::wait_frame = sync_node.value<bool>("wait_frame", graphic::wait_frame);
  graphic::set_disable_frame_limit( sync_node.value<bool>("disable_frame_limit", graphic::get_disable_frame_limit()) );
  graphic::set_target_fps( sync_node.get_int("target_fps", graphic::get_target_vsync_fps()) );
  graphic::cpu_safe = sync_node.value<bool>("cpu_safe", graphic::cpu_safe);
  graphic::autoopt_timeout_max = sync_node.get_real("autoopt_timeout_max", graphic::autoopt_timeout_max);

  #ifndef NO_EPGE
  cauto epge_node = config["epge"];
  load_epges(epge_node);
  #endif
}

void save_config() {
  auto& config = hpw::config;

  config.set_bool("first_start", hpw::first_start);
  config.set_bool("enable_replay", hpw::enable_replay);
  config.set_bool("need_tutorial", hpw::need_tutorial);

  auto game_node = make_node(config, "game");
  save_game_config(game_node);
  save_nickname();

  auto path_node = make_node(config, "path");
  path_node.set_str("screenshots", hpw::screenshots_path);
  path_node.set_str("resources", hpw::data_path);
  path_node.set_str("os_resources_dir", hpw::os_resources_dir);
  path_node.set_str("replays_dir", hpw::replays_path);

  auto debug = make_node(config, "debug");
  debug.set_bool("empty_level_first", hpw::empty_level_first);
  debug.set_str ("start_script", hpw::start_script);

  auto graphic_node = make_node(config, "graphic");
  graphic_node.set_v_int("canvas_size",        {graphic::width, graphic::height} );
  graphic_node.set_int  ("light_quality",       scast<int>(graphic::light_quality) );
  graphic_node.set_bool ("enable_motion_interp",graphic::enable_motion_interp);
  graphic_node.set_bool ("fullscren",           graphic::fullscreen);
  graphic_node.set_bool ("draw_border",         graphic::draw_border);
  graphic_node.set_bool ("show_mouse_cursour",  graphic::show_mouse_cursour);
  graphic_node.set_int  ("resize_mode",         scast<int>(graphic::resize_mode) );
  graphic_node.set_int  ("motion_blur_mode",    scast<int>(graphic::motion_blur_mode) );
  graphic_node.set_int  ("blur_mode",           scast<int>(graphic::blur_mode) );
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

  #ifndef NO_EPGE
  auto epge_node = make_node(graphic_node, "epge");
  save_epges(epge_node);
  #endif

  auto log_node = make_node(config, "log");
  save_log_config(log_node);

  auto sync_node = make_node(graphic_node, "sync");
  sync_node.set_bool("vsync",               graphic::get_vsync());
  sync_node.set_bool("wait_frame",          graphic::wait_frame);
  sync_node.set_int ("target_fps",          graphic::get_target_fps());
  sync_node.set_bool("cpu_safe",            graphic::cpu_safe);
  sync_node.set_real("autoopt_timeout_max", graphic::autoopt_timeout_max);
  sync_node.set_bool("disable_frame_limit", graphic::get_disable_frame_limit());

  auto input_node = make_node(config, "input");
  #define SAVE_KEY(name) input_node[#name] = get_scancode(hpw::keycode::name);
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
  save(config, hpw::cur_dir + hpw::config_path, true);
}

void load_config() {
  log_info << "чтение конфига...";
  make_dir_if_not_exist(hpw::cur_dir + hpw::config_dir);
  load(hpw::config, hpw::cur_dir + hpw::config_path, true);
  log_info << "файл конфига: \"" + hpw::cur_dir + hpw::config_path + "\"";

  crauto config = hpw::config;
  hpw::first_start = config.value<bool>("first_start", true);
  hpw::enable_replay = config.value<bool>("enable_replay", hpw::enable_replay);
  hpw::need_tutorial = config.value<bool>("need_tutorial", hpw::need_tutorial);
  
  cauto debug = config["debug"];
  hpw::empty_level_first = debug.value<bool>("empty_level_first", hpw::empty_level_first);
  hpw::start_script = debug.value<Str>("start_script", hpw::start_script);

  cauto path_node = config["path"];
  hpw::screenshots_path = path_node.value<Str>("screenshots", hpw::screenshots_path);
  hpw::data_path = path_node.value<Str>("resources", hpw::data_path);
  hpw::os_resources_dir = path_node.value<Str>("os_resources_dir", hpw::os_resources_dir);
  hpw::replays_path = path_node.value<Str>("replays_dir", hpw::replays_path);

  // сделать папки, если их нет
  make_dir_if_not_exist(hpw::cur_dir + path_node.value<Str>("screenshots", "screenshots"));
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
}
