#include <sstream>
#include <ctime>
#include "scene-game.hpp"
#include "scene-loading.hpp"
#include "scene-debug.hpp"
#include "scene-manager.hpp"
#include "util/str-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/random.hpp"
#include "host/command.hpp"
#include "game/game-core.hpp"
#include "game/game-sync.hpp"
#include "game/game-canvas.hpp"
#include "game/game-common.hpp"
#include "game/game-debug.hpp"
#include "game/game-graphic.hpp"
#include "game/game-font.hpp"
#include "game/game-camera.hpp"
#include "game/util/game-replay.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/util/post-effect/game-post-effects.hpp"
#include "game/util/post-effect/post-effects.hpp"
#include "game/util/camera.hpp"
#include "game/util/replay.hpp"
#include "game/util/score-table.hpp"
#include "game/entity/util/mem-map.hpp"
#include "game/hud/hud.hpp"
#include "game/scene/scene-game-pause.hpp"
#include "game/scene/scene-gameover.hpp"
#include "game/level/level-manager.hpp"
#include "game/level/level-replay-test.hpp"
#include "game/level/level-space.hpp"
#include "game/level/level-1.hpp"
#include "game/level/level-tutorial.hpp"
#ifdef DEBUG
#include "game/level/level-debug.hpp"
#include "game/level/level-debug-1.hpp"
#include "game/level/level-debug-2.hpp"
#include "game/level/level-debug-3.hpp"
#include "game/level/level-debug-bullets.hpp"
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

void Scene_game::init_levels() {
  hpw::level_mgr = new_shared<Level_mgr>(Level_mgr::Makers{
    [] { return new_shared<Level_tutorial>(); },
    [] { return new_shared<Level_space>(); },
    //[] { return new_shared<Level_1>(); },
    
  #ifdef DEBUG
    //[] { return new_shared<Level_debug_bullets>(); },
    //[] { return new_shared<Level_debug_3>(); },
    //[] { return new_shared<Level_replay_test>(); },
    //[] { return new_shared<Level_debug_1>(); },
    //[] { return new_shared<Level_debug>(); },
    //[] { return new_shared<Level_debug_2>(); },
  #endif
  }); // init level order
} // init_levels

Scene_game::Scene_game(): death_timer {4} {
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
}

Scene_game::~Scene_game() {
  hpw::entity_mgr = {};
  if (graphic::get_fast_forward())
    graphic::set_fast_forward(false);
}

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

  replay_save_keys();
  replay_load_keys();

  hpw::level_mgr->update(get_level_vel(), dt);
  hpw::entity_mgr->update(dt);
  graphic::camera->update(dt);
  graphic::post_effects->update(dt);
  check_death(dt);
  
  ++hpw::game_updates_safe;
} // update

void Scene_game::draw(Image& dst) const {
  hpw::level_mgr->draw(dst);
  hpw::entity_mgr->draw(dst, graphic::camera->get_offset());
  hpw::level_mgr->draw_upper_layer(dst);
  graphic::post_effects->draw(dst);
  draw_hud(dst);
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
    replay = new_unique<Replay>(hpw::cur_dir + "replays/" + get_random_replay_name(), true);
} // replay_init

void Scene_game::replay_save_keys() {
  return_if(!hpw::enable_replay);
  return_if(hpw::replay_read_mode);
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
  check_key(hpw::keycode::shoot)
  #undef check_key

  replay->push( std::move(packet) );
}

void Scene_game::replay_load_keys() {
  return_if (!hpw::replay_read_mode);

  // сбросить свои клавиши
  clear_cur_keys();

  // прочитать клавиши с реплея
  auto key_packet = replay->pop();
  if (key_packet) {
    for (cnauto key: *key_packet)
      press(key);
  } else {
    // по завершению реплея выходить из сцены обратно
    hpw_log("replay end\n");
    hpw::replay_read_mode = false;
    hpw::scene_mgr->back();
  }
} // replay_load_keys

void Scene_game::check_death(double dt) {
  // перезапуск уровня, если игрок умер
  if (cauto player = hpw::entity_mgr->get_player(); player)
    if ( !player->status.live)
      if (death_timer.update(dt)) // по завершению таймера
        hpw::scene_mgr->add( new_shared<Scene_gameover>() );
}
