#include "scene-gameover.hpp"
#include "scene-manager.hpp"
#include "scene-loading.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/util/game-replay.hpp"
#include "game/util/replay.hpp"
#include "game/util/locale.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/util/game-util.hpp"

struct Scene_gameover::Impl {
  Unique<Menu> menu {};

  inline Impl() {
    init_menu();
  }

  inline void update(double dt) {
    if (hpw::replay_read_mode) {
      hpw::scene_mgr->back(); // to gameover
      hpw::scene_mgr->back(); // to replay-select
    }

    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    menu->draw(dst);
    // TODO locale
    graphic::font->draw(dst, {30, 40}, U"GAME OVER", &blend_max);
  }

  inline void init_menu() {
    menu = new_unique<Text_menu>(
      Menu_items {
        
        new_shared<Menu_text_item>( get_locale_str("scene.gameover.try_again"), []{
          hpw::scene_mgr->back(); // to game
          hpw::scene_mgr->back(); // to load screen
          hpw::scene_mgr->back(); // to difficulty menu
          // перезапуск игры
          hpw::scene_mgr->add(new_shared<Scene_loading>( []{
            hpw::replay_read_mode = false;
            hpw::scene_mgr->add(new_shared<Scene_game>());
          } ));
        } ),

        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{
          hpw::scene_mgr->back(); // to game
          hpw::scene_mgr->back(); // to load screen
          hpw::scene_mgr->back(); // to difficulty menu
          hpw::scene_mgr->back(); // to main menu
        }),
      },

      Vec{60, 80}
    );
  } // init_menu

}; // impl

Scene_gameover::Scene_gameover(): impl {new_unique<Impl>()} {}
Scene_gameover::~Scene_gameover() {}
void Scene_gameover::update(double dt) { impl->update(dt); }
void Scene_gameover::draw(Image& dst) const { impl->draw(dst); }
