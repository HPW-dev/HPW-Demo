#include "scene-mgr.hpp"
#include "scene-loading.hpp"
#include "scene-gameover.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/replays.hpp"
#include "game/core/sounds.hpp"
#include "game/util/replay.hpp"
#include "game/util/locale.hpp"
#include "game/util/palette-helper.hpp"
#include "game/scene/scene-game.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "graphic/image/image.hpp"
#include "util/math/random.hpp"

struct Scene_gameover::Impl {
  Unique<Menu> menu {};

  inline explicit Impl() {
    init_menu();

    // поменять палитру
    if (hpw::rnd_pal_after_death)
      randomize_palette();
    
    // выключить звуки, что были до этого
    assert(hpw::sound_mgr);
    hpw::sound_mgr->shutup();
  }

  inline void update(const Delta_time dt) {
    if (hpw::replay_read_mode)
      hpw::scene_mgr.back(2); // cur->gameover->replay-select

    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    menu->draw(dst);
  }

  inline void init_menu() {
    Menu_items items;
    items.push_back (
      new_shared<Menu_text_item>( get_locale_str("gameover.try_again"), []{
        hpw::scene_mgr.back(3); // cur->game->load screen->difficulty menu
        // перезапуск игры
        hpw::scene_mgr.add(new_shared<Scene_loading>( []{
          hpw::replay_read_mode = false;
          hpw::scene_mgr.add(new_shared<Scene_game>());
        } ));
      } )
    );

    if (hpw::enable_replay) {
      items.push_back (
        new_shared<Menu_bool_item>( get_locale_str("gameover.save_replay_question"),
          []{ return hpw::save_last_replay; },
          [](const bool new_val) { hpw::save_last_replay = new_val; },
          get_locale_str("gameover.descriptin.save_replay_question")
        )
      );
    } // if enable_replay
    
    // менять палитру при смерти
    items.push_back( new_shared<Menu_bool_item>(
      get_locale_str("game_opts.rnd_pal.title_after_death"),
      []{ return hpw::rnd_pal_after_death; },
      [](bool val) { 
        if (val)
          randomize_palette();
        hpw::rnd_pal_after_death = val;
      },
      get_locale_str("game_opts.rnd_pal.desc")
    ) );

    items.push_back ( new_shared<Menu_text_item>(get_locale_str("common.exit"), [this]{ this->exit(); }) );
    // понаделать кучу выходов со входом в секретку
    if (hpw::difficulty != Difficulty::easy)
      cfor (_, 26)
        items.push_back ( new_shared<Menu_text_item>(get_locale_str("common.exit"), [this]{ this->exit(); }) );

    init_unique<Advanced_text_menu>( menu,
      get_locale_str("gameover.title"),
      items, Rect(0,0, graphic::width, graphic::height) );
  } // init_menu

  inline void exit() {
    if (hpw::difficulty != Difficulty::easy) {
      if (rndr() <= (1.f / 6.f)) {
        hpw_log("вам повезло зайти в секретный уровень, но он ещё не реализован!\n");
        // TODO call scene secret_level
        // return;
      }
    }

    hpw::scene_mgr.back(4); // cur -> game -> load screen -> difficulty menu -> main menu
  }
}; // impl

Scene_gameover::Scene_gameover(): impl {new_unique<Impl>()} {}
Scene_gameover::~Scene_gameover() {}
void Scene_gameover::update(const Delta_time dt) { impl->update(dt); }
void Scene_gameover::draw(Image& dst) const { impl->draw(dst); }
