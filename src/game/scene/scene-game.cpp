#include <utility>
#include <algorithm>
#include <ctime>
#include "scene-game.hpp"
#include "scene-loading.hpp"
#include "scene-mgr.hpp"
#include "host/command.hpp"
#include "sound/sound-mgr.hpp"
#include "util/str-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "util/log.hpp"
#include "game/core/core.hpp"
#include "game/core/user.hpp"
#include "game/core/common.hpp"
#include "game/core/sounds.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/fonts.hpp"
#include "game/core/entities.hpp"
#include "game/core/replays.hpp"
#include "game/core/levels.hpp"
#include "game/core/scenes.hpp"
#include "game/core/messages.hpp"
#include "game/core/huds.hpp"
#include "game/core/tasks.hpp"
#include "game/core/debug.hpp"
#include "game/hud/hud-util.hpp"
#include "game/util/sync.hpp"
#include "game/util/replay-check.hpp"
#include "game/util/virt-joystick.hpp"
#include "game/util/keybits.hpp"
#include "game/util/camera.hpp"
#include "game/util/animation-helper.hpp"
#include "game/util/score-table.hpp"
#include "game/util/post-effect/game-post-effects.hpp"
#include "game/util/post-effect/post-effects.hpp"
#include "game/util/cmd/cmd.hpp"
#include "game/scene/scene-game-pause.hpp"
#include "game/level/level-space.hpp"
#include "game/level/level-tutorial.hpp"
#include "game/entity/player/player-dark.hpp"
#include "game/entity/util/mem-map.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/collider/collider.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

#ifdef DEBUG
#include "scene-cmd.hpp"
#include "scene-debug.hpp"
#include "game/util/cmd/cmd-script.hpp"
#include "game/util/dbg-plots.hpp"
#include "game/level/level-empty.hpp"
//#include "game/level/level-collision-test.hpp"
//#include "game/level/level-debug-1.hpp"
//#include "game/level/level-debug-2.hpp"
//#include "game/level/level-debug-3.hpp"
//#include "game/level/level-debug-bullets.hpp"
#else
#include "game/scene/scene-graphic.hpp"
#endif

void Scene_game::init_levels() {
  // запустить только один уровень при отладке
  #ifdef DEBUG
  if (hpw::empty_level_first) {
    init_unique(hpw::level_mgr, Level_mgr::Makers {
      []{ return new_shared<Level_empty>(); }
    } ); 
    return;
  }
  #endif

  if (m_start_tutorial) { // начать с туториала
    init_unique(hpw::level_mgr, Level_mgr::Makers{
      [] { return new_shared<Level_tutorial>(); }
    });
    return;
  }

  // стандартный порядок уровней
  init_unique(hpw::level_mgr, Level_mgr::Makers{
    [] { return new_shared<Level_space>(); },
    //[] { return new_shared<Level_1>(); },
    #ifdef DEBUG
    //[] { return new_shared<Level_debug_bullets>(); },
    //[] { return new_shared<Level_debug_1>(); },
    //[] { return new_shared<Level_debug_2>(); },
    //[] { return new_shared<Level_debug_3>(); },
    //[] { return new_shared<Level_empty>(); },
    //[] { return new_shared<Level_collision_test>(); },
    #endif
  }); // init level order
} // init_levels

struct Scene_game::Impl {
  Scene_game& m_master;
  #ifdef DEBUG
  Dbg_plots m_dbg_plots {};
  #endif

  inline explicit Impl(Scene_game& master): m_master {master} {}

  inline void update(Delta_time dt) {
    #ifdef DEBUG
    if (graphic::draw_plots)
      m_dbg_plots.update(dt);
    #endif
  }

  inline void post_draw(Image& dst) const {
    #ifdef DEBUG
    if (graphic::draw_plots)
      m_dbg_plots.draw(dst);
    #endif
  }
}; // Impl

Scene_game::Scene_game(const bool start_tutorial)
: m_start_tutorial {start_tutorial}
, m_impl {new_unique<Impl>(*this)}
{
  hpw::first_level_is_tutorial = start_tutorial;

  // -------------- [!] ----------------
  replay_init(); // не перемещать вниз, тут грузится сид
  // -------------- [!] ----------------

  init_shared<Effect_mgr>(graphic::post_effects);
  init_entitys();
  load_animations();

  hpw::entity_mgr->register_types();
  init_levels();
  init_shared(graphic::camera);
  graphic::render_lag = false; // когда игра грузиться, она думает что лагает
  // на средних настройках закэшировать вспышки перед запуском игры
  if (graphic::light_quality == Light_quality::medium)
    cache_light_spheres();
  graphic::hud = make_hud(graphic::cur_hud);
  hpw::save_last_replay = false;
  hpw::sound_mgr->shutup();
  init_unique(hpw::message_mgr);
  startup_script();
} // c-tor

Scene_game::~Scene_game() {
  hpw::sound_mgr->shutup();
  if (graphic::get_fast_forward())
    graphic::set_fast_forward(false);
  graphic::post_effects = {};
  hpw::entity_mgr = {};
  graphic::camera = {};
  hpw::hitbox_layer = {};
  hpw::level_mgr = {};
  if (hpw::enable_replay)
    save_named_replay();
} // d-tor

void Scene_game::update(const Delta_time dt) {
  assert(dt == hpw::target_tick_time);
  m_impl->update(dt);

  // input:
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr.add(new_shared<Scene_game_pause>());
  #ifdef DEBUG
  if (is_pressed_once(hpw::keycode::debug))
    hpw::scene_mgr.add(new_shared<Scene_debug>());
  if (is_pressed_once(hpw::keycode::console))
    hpw::scene_mgr.add(new_shared<Scene_cmd>());
  if (is_pressed_once(hpw::keycode::fast_forward))
    graphic::set_fast_forward( !graphic::get_fast_forward() );
  #endif

  hpw::level_mgr->update(get_level_vel(), dt);
  if (hpw::level_mgr->end_of_levels) {
    hpw::scene_mgr.back(3); // cur->loading screen->diffuculty->main menu
    hpw_log("уровни кончились, выход из сцены игры\n", Log_stream::debug);
  }
  hpw::task_mgr.update(dt);

  if (graphic::hud)
    graphic::hud->update(dt);
  hpw::entity_mgr->update(dt);
  graphic::camera->update(dt);
  graphic::post_effects->update(dt);
  hpw::message_mgr->update(dt);
  
  ++hpw::game_ticks;

  #ifdef STABLE_REPLAY
    if ((hpw::game_ticks % 72) == 0)
      replay_stable_log();
    /*if (hpw::game_ticks >= 6'000)
      hpw::soft_exit();*/
  #endif
} // update

void Scene_game::draw(Image& dst) const {
  hpw::level_mgr->draw(dst);
  hpw::task_mgr.draw_post_bg(dst);
  hpw::message_mgr->draw(dst);
  hpw::entity_mgr->draw(dst, graphic::camera->get_offset());
  hpw::level_mgr->draw_upper_layer(dst);
  graphic::post_effects->draw(dst);
  hpw::task_mgr.draw(dst);
  if (graphic::hud)
    graphic::hud->draw(dst);
  post_draw(dst);
} // draw

void Scene_game::startup_script() {
  if (!hpw::start_script.empty())
    hpw::cmd.exec("script " + hpw::cur_dir + hpw::start_script);
}

void Scene_game::init_entitys() {
  // для показа хитбоксов
  init_shared(hpw::hitbox_layer, graphic::canvas->X, graphic::canvas->Y);
  init_unique(hpw::entity_mgr);
}

void Scene_game::post_draw(Image& dst) const {
  if (graphic::draw_hitboxes) { // показать хитбоксы
    insert<&blend_no_black>(dst, *hpw::hitbox_layer, {});
    hpw::hitbox_layer->fill(Pal8::black);
  }
  if (graphic::show_grids) // сетки системы коллизий
    hpw::entity_mgr->debug_draw(dst);
  if (graphic::show_virtual_joystick) // нажимаемые кнопки
    draw_virtual_joystick(dst);
  if (hpw::show_entity_mem_map) // память занятая объектами
    draw_entity_mem_map(dst, Vec(5, 5));
  #ifdef STABLE_REPLAY
    graphic::font->draw(dst, Vec(5, 5), U"game updates: " +
      n2s<utf32>(hpw::game_ticks), &blend_diff);
  #endif
  m_impl->post_draw(dst);
} // post_draw

Vec Scene_game::get_level_vel() const {
  auto player = hpw::entity_mgr->get_player();
  if (player)
    return player->phys.get_vel();
  return {};
}

void Scene_game::replay_init() {
  clear_cur_keys();
  // globals
  hpw::game_ticks = 0;
  hpw::any_key_pressed = false;
  hpw::cur_upf = 0;
  hpw::cur_ups = 0;
  clear_entity_uid();
  hpw::clear_score();
  // random
  set_rnd_seed( time({}) );
  hpw_log("назначен новый сид рандома: " + n2s(get_rnd_seed()) + '\n', Log_stream::debug);

  Str rep_path;
  if (hpw::replay_read_mode) {
    rep_path = hpw::cur_replay_file_name;
    init_unique(hpw::replay, rep_path, false);
  } elif (hpw::enable_replay) {
    rep_path = hpw::cur_dir + hpw::replays_path + "last_replay.hpw_replay";
    init_unique(hpw::replay, rep_path, true);
  }
  
  // если есть ошибки, показать их
  if (cauto warnings = hpw::replay->warnings(); !warnings.empty()) {
    hpw_log("проблемы с реплеем \"" + rep_path + "\":\n" + utf32_to_8(warnings), Log_stream::warning);
    hpw::user_warnings += warnings;
  }
} // replay_init

void Scene_game::save_named_replay() {
  assert(hpw::enable_replay);
  return_if(!hpw::replay);

  try {
    // реплей сейвится при закрытии
    hpw::replay->close();
    hpw::replay = {};

    if (hpw::save_last_replay) {
      /*
      // открыть файл последнего реплея и скопировать в именной файл
      std::ifstream source(hpw::cur_dir + hpw::replays_path + "last_replay.hpw_replay", std::ios::binary);
      auto replay_name = hpw::cur_dir + hpw::replays_path + get_random_replay_name();
      conv_sep(replay_name);
      replay_name = to_safe_fname(replay_name);

      hpw_log("сохранение файла реплея \"" + replay_name + "\"\n");
      std::ofstream dest(replay_name, std::ios::binary);
      iferror(!dest.is_open(), "не получилось переименовать реплей");
      dest << source.rdbuf();
      */
      error("need impl"); // TODO
    }
  } catch (cr<hpw::Error> err) {
    hpw::user_warnings = U"ошибка при сохранении реплея: " + utf8_to_32(err.what()) + U"\n";
  } catch (...) {
    hpw::user_warnings = U"неизвестная ошибка при сохранении реплея\n";
  }
}
