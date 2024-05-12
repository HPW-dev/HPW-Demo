#include <utility>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <ctime>
#include "scene-game.hpp"
#include "scene-loading.hpp"
#include "scene-debug.hpp"
#include "scene-manager.hpp"
#include "host/command.hpp"
#include "util/str-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "game/core/shop.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/debug.hpp"
#include "game/core/graphic.hpp"
#include "game/core/fonts.hpp"
#include "game/core/entities.hpp"
#include "game/core/replays.hpp"
#include "game/core/levels.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sounds.hpp"
#include "game/core/huds.hpp"
#include "game/util/sync.hpp"
#include "game/util/replay-check.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/util/post-effect/game-post-effects.hpp"
#include "game/util/post-effect/post-effects.hpp"
#include "game/util/camera.hpp"
#include "game/util/replay.hpp"
#include "game/util/score-table.hpp"
#include "game/util/game-shop-debug.hpp"
#include "game/entity/util/mem-map.hpp"
#include "game/hud/hud-asci.hpp"
#include "game/scene/scene-game-pause.hpp"
#include "game/level/level-manager.hpp"
#include "game/level/level-space.hpp"
#include "game/level/level-tutorial.hpp"
//#include "game/level/level-1.hpp"
#ifdef DEBUG
//#include "game/level/level-debug.hpp"
//#include "game/level/level-debug-1.hpp"
//#include "game/level/level-debug-2.hpp"
//#include "game/level/level-debug-3.hpp"
#include "game/level/level-debug-4.hpp"
//#include "game/level/level-debug-bullets.hpp"
#endif
#include "game/entity/entity-manager.hpp"
#include "game/entity/player-dark.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/collider/collider.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "sound/sound-manager.hpp"

void Scene_game::init_levels() {
  if (m_start_tutorial) { // начать с туториала
    hpw::level_mgr = new_shared<Level_mgr>( Level_mgr::Makers{
      [] { return new_shared<Level_tutorial>(); }
    });
  } else {
    hpw::level_mgr = new_shared<Level_mgr>( Level_mgr::Makers{
      [] { return new_shared<Level_space>(); },
      //[] { return new_shared<Level_1>(); },
      #ifdef DEBUG
      //[] { return new_shared<Level_debug_bullets>(); },
      //[] { return new_shared<Level_debug_1>(); },
      //[] { return new_shared<Level_debug_2>(); },
      //[] { return new_shared<Level_debug_3>(); },
      //[] { return new_shared<Level_debug_4>(); },
      //[] { return new_shared<Level_debug>(); },
      #endif
    }); // init level order
  }
} // init_levels

Scene_game::Scene_game(const bool start_tutorial)
: m_start_tutorial {start_tutorial} {
  hpw::first_level_is_tutorial = start_tutorial;

  // -------------- [!] ----------------
  replay_init(); // не перемещать вниз, тут грузится сид
  // -------------- [!] ----------------

  graphic::post_effects = new_shared<Effect_mgr>();
  init_entitys();
  load_animations();
  hpw::entity_mgr->register_types();
  init_levels();
  graphic::camera = new_shared<Camera>();
  graphic::render_lag = false; // когда игра грузиться, она думает что лагает
  // на средних настройках закэшировать вспышки перед запуском игры
  if (graphic::light_quality == Light_quality::medium)
    cache_light_spheres();
  // TODO выбор HUD с конфига
  graphic::hud = new_shared<Hud_asci>();
  hpw::save_last_replay = false;
  init_sound_system();
}

Scene_game::~Scene_game() {
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

void Scene_game::update(double dt) {
  assert(dt == hpw::target_update_time);

  // input:
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->add(new_shared<Scene_game_pause>());
  #ifdef DEBUG
  if (is_pressed_once(hpw::keycode::debug))
    hpw::scene_mgr->add(new_shared<Scene_debug>());
  if (is_pressed_once(hpw::keycode::fast_forward))
    graphic::set_fast_forward( !graphic::get_fast_forward() );
  #endif

  if (hpw::replay_read_mode)
    replay_load_keys();
  else if (hpw::enable_replay)
    replay_save_keys();

  if (hpw::shop) {
    if (!hpw::shop->update(dt))
      hpw::shop = {};
    return;
  }

  hpw::level_mgr->update(get_level_vel(), dt);
  if (hpw::level_mgr->end_of_levels) {
    hpw::scene_mgr->back(); // exit to loading screen
    hpw::scene_mgr->back(); // exit to diffuculty
    hpw::scene_mgr->back(); // exit to main menu
    detailed_log("уровни кончились, выход из сцены игры\n");
  }

  if (graphic::hud)
    graphic::hud->update(dt);
  hpw::entity_mgr->update(dt);
  graphic::camera->update(dt);
  graphic::post_effects->update(dt);
  
  ++hpw::game_updates_safe;

  #ifdef STABLE_REPLAY
    if ((hpw::game_updates_safe % 72) == 0)
      replay_stable_log();
    /*if (hpw::game_updates_safe >= 6'000)
      hpw::soft_exit();*/
  #endif
} // update

void Scene_game::draw(Image& dst) const {
  if (hpw::shop) {
    hpw::shop->draw(dst);
    return;
  }

  hpw::level_mgr->draw(dst);
  hpw::entity_mgr->draw(dst, graphic::camera->get_offset());
  hpw::level_mgr->draw_upper_layer(dst);
  graphic::post_effects->draw(dst);
  if (graphic::hud)
    graphic::hud->draw(dst);
  post_draw(dst);
} // draw

void Scene_game::init_entitys() {
  // для показа хитбоксов
  hpw::hitbox_layer = new_shared<Image>(graphic::canvas->X, graphic::canvas->Y);
  hpw::entity_mgr = new_shared<Entity_mgr>();
}

void Scene_game::post_draw(Image& dst) const {
  if (graphic::draw_hitboxes) { // показать хитбоксы
    insert<&blend_no_black>(dst, *hpw::hitbox_layer, {});
    hpw::hitbox_layer->fill(Pal8::black);
  }
  if (graphic::show_grids) // сетки системы коллизий
    hpw::entity_mgr->debug_draw(dst);
  if (graphic::show_debug_info) // отобразить фпс
    draw_debug_info(dst);
  if (graphic::draw_controls) // нажимаемые кнопки
    draw_controls(dst);
  if (hpw::show_entity_mem_map) // память занятая объектами
    draw_entity_mem_map(dst, Vec(5, 5));
  #ifdef STABLE_REPLAY
    graphic::font->draw(dst, Vec(5, 5), U"game updates: " +
      n2s<utf32>(hpw::game_updates_safe), &blend_diff);
  #endif
} // post_draw

void Scene_game::draw_debug_info(Image& dst) const {
  std::stringstream txt;
  txt << "real dt: " << n2s(hpw::real_dt, 12);
  txt << "\nsafe dt: " << n2s(hpw::safe_dt, 12);
  txt << "\ndraw time: " << n2s(graphic::soft_draw_time, 12);
  txt << "\nupdate time: " << n2s(hpw::update_time_unsafe, 12);
  txt << "\nups: " << n2s(hpw::cur_ups, 1);
  txt << "\nfps: " << n2s(graphic::cur_fps, 1);
  auto str_u32 = sconv<utf32>(txt.str());
  // нарисовать чёрный прямоуг под текст
  auto pos_x = 20;
  auto pos_y = 20;
  auto border = 8;
  static int w; // чтоб по ширине не дёргалось окно
  w = std::max(w, graphic::font->text_width(str_u32));
  int h = graphic::font->text_height(str_u32);
  Rect rect {
    pos_x - border,
    pos_y - border,
    w + border + 6,
    h + border + 12
  };
  draw_rect_filled<&blend_158>(dst, rect, Pal8::black);
  draw_rect<&blend_diff>(dst, rect, Pal8::white);
  graphic::font->draw(dst, {pos_x, pos_y}, str_u32);
} // draw_debug_info

Vec Scene_game::get_level_vel() const {
  auto player = hpw::entity_mgr->get_player();
  if (player)
    return player->phys.get_vel();
  return {};
}

void Scene_game::replay_init() {
  clear_cur_keys();
  // globals
  hpw::game_updates_safe = 0;
  hpw::any_key_pressed = false;
  hpw::cur_upf = 0;
  hpw::cur_ups = 0;
  clear_entity_uid();
  hpw::clear_score();
  // random
  set_rnd_seed( time({}) );
  #ifdef DETAILED_LOG
    uint32_t seed = get_rnd_seed();
    detailed_log("назначен новый сид рандома: " << seed << '\n');
  #endif

  if (hpw::replay_read_mode)
    replay = new_unique<Replay>(hpw::cur_replay_file_name, false);
  else if (hpw::enable_replay)
    replay = new_unique<Replay>(hpw::cur_dir + "replays/last_replay.hpw_replay", true);
} // replay_init

void Scene_game::replay_save_keys() {
  Key_packet packet;

  #define check_key(key) if (is_pressed(key)) \
    packet.emplace_back(key);
  check_key(hpw::keycode::up)
  check_key(hpw::keycode::down)
  check_key(hpw::keycode::left)
  check_key(hpw::keycode::right)
  check_key(hpw::keycode::enable)
  check_key(hpw::keycode::focus)
  check_key(hpw::keycode::mode)
  check_key(hpw::keycode::bomb)
  check_key(hpw::keycode::shoot)
  #undef check_key

  replay->push( std::move(packet) );
}

void Scene_game::replay_load_keys() {
  // сбросить свои клавиши
  clear_cur_keys();
  hpw::any_key_pressed = false;

  // прочитать клавиши с реплея
  auto key_packet = replay->pop();
  if (key_packet) {
    for (cnauto key: *key_packet) {
      press(key);
      hpw::any_key_pressed = true;
    }
  } else {
    // по завершению реплея выходить из сцены обратно
    hpw_log("replay end\n");
    hpw::replay_read_mode = false;
    hpw::scene_mgr->back();
  }
} // replay_load_keys

void Scene_game::save_named_replay() {
  assert(hpw::enable_replay);
  try {
    // реплей сейвится при закрытии
    replay->close();
    if (hpw::save_last_replay) {
      // открыть файл последнего реплея и скопировать в именной файл
      std::ifstream source(hpw::cur_dir + "replays/last_replay.hpw_replay", std::ios::binary);
      std::ofstream dest(hpw::cur_dir + "replays/" + get_random_replay_name(), std::ios::binary);
      dest << source.rdbuf();
    }
  } catch (...) {
    // TODO окно с ошибкой
    hpw_log("ошибка при сохранении реплея\n");
  }
}

void Scene_game::init_sound_system() {
  // TODO применение настроек при создании
  hpw::sound_mgr = new_unique<Sound_mgr>();
  load_sounds();
}
