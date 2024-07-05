#include "scene-mgr.hpp"
#include "scene-loading.hpp"
#include "scene-gameover.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/replays.hpp"
#include "game/util/replay.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/scene/scene-game.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "graphic/image/image.hpp"

struct Scene_gameover::Impl {
  Unique<Menu> menu {};

  inline explicit Impl() {
    init_menu();

    // поменять палитру
    if (hpw::rnd_pal_after_death && !hpw::replay_read_mode)
      set_random_palette();
  }

  inline void update(const Delta_time dt) {
    if (hpw::replay_read_mode) {
      hpw::scene_mgr->back(); // to gameover
      hpw::scene_mgr->back(); // to replay-select
    }

    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    menu->draw(dst);
  }

  inline void init_menu() {
    Menu_items items;
    items.push_back (
      new_shared<Menu_text_item>( get_locale_str("scene.gameover.try_again"), []{
        hpw::scene_mgr->back(); // to game
        hpw::scene_mgr->back(); // to load screen
        hpw::scene_mgr->back(); // to difficulty menu
        // перезапуск игры
        hpw::scene_mgr->add(new_shared<Scene_loading>( []{
          hpw::replay_read_mode = false;
          hpw::scene_mgr->add(new_shared<Scene_game>());
        } ));
      } )
    );

    if (hpw::enable_replay) {
      items.push_back (
        new_shared<Menu_bool_item>( get_locale_str("scene.gameover.save_replay_question"),
          []{ return hpw::save_last_replay; },
          [](const bool new_val) { hpw::save_last_replay = new_val; },
          get_locale_str("scene.gameover.descriptin.save_replay_question")
        )
      );
    } // if enable_replay
    
    // менять палитру при смерти
    items.push_back( new_shared<Menu_bool_item>(
      get_locale_str("scene.game_opts.rnd_pal.title"),
      []{ return hpw::rnd_pal_after_death; },
      [](bool val) { 
        if (val)
          set_random_palette();
        hpw::rnd_pal_after_death = val;
      },
      get_locale_str("scene.game_opts.rnd_pal.desc")
    ) );

    items.push_back (
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
        hpw::scene_mgr->back(); // to game
        hpw::scene_mgr->back(); // to load screen
        hpw::scene_mgr->back(); // to difficulty menu
        hpw::scene_mgr->back(); // to main menu
      })
    );
    menu = new_unique<Advanced_text_menu>( get_locale_str("scene.gameover.title"),
      items, Rect(0,0, graphic::width, graphic::height) );
  } // init_menu

}; // impl

Scene_gameover::Scene_gameover(): impl {new_unique<Impl>()} {}
Scene_gameover::~Scene_gameover() {}
void Scene_gameover::update(const Delta_time dt) { impl->update(dt); }
void Scene_gameover::draw(Image& dst) const { impl->draw(dst); }
