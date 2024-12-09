#include <cassert>
#include "scene-palette-select.hpp"
#include "scene-test-image.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/palette.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sprites.hpp"
#include "game/core/graphic.hpp"
#include "game/core/palette.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/palette-helper.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/scene/scene-game.hpp"
#include "util/file/archive.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "host/command.hpp"

struct Scene_palette_select::Impl {
  Unique<Menu> _menu {};
  Shared<Sprite> _test_image {};

  inline Impl() {
    init_menu();
    _test_image = hpw::sprites.find("resource/image/other/palette test.png");
    assert(_test_image);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
  }

  inline void draw(Image& dst) const {;
    draw_test_image(dst);
    _menu->draw(dst);
    draw_palette(dst, Vec(35, 110));
  }

  inline void init_menu() {
    Text_menu_config menu_config;
    menu_config.with_bg = true;

    init_unique<Text_menu>( _menu,
      Menu_items {
        get_palette_list(),
        get_test_image_list(),

        new_shared<Menu_text_item>(get_locale_str("common.reset"), [this]{ 
          graphic::current_palette_file = Str{graphic::DEFAULT_PALETTE_FILE};
          hpw::init_palette_from_archive(graphic::current_palette_file);
          graphic::cur_test_image_path = graphic::DEFAULT_TEST_IMAGE;
          init_menu();
        }),
        
        new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }),
      },

      Vec{30, 25}, menu_config
    );
  } // init_menu

  inline void draw_palette(Image& dst, const Vec pos) const {
    cauto pal_w = 256;
    cauto pal_h = 11;
    cfor (x, pal_w)
      cfor (y, pal_h)
        dst.set(x + pos.x, y + pos.y, Pal8(x), {});
    // рамка вокруг палитры
    draw_rect(dst, Rect(pos - Vec(1, 1), Vec(pal_w, pal_h) + Vec(2, 2)), Pal8::black);
    draw_rect(dst, Rect(pos - Vec(2, 2), Vec(pal_w, pal_h) + Vec(4, 4)), Pal8::white);
  }
}; // impl

Scene_palette_select::Scene_palette_select(): impl {new_unique<Impl>()} {}
Scene_palette_select::~Scene_palette_select() {}
void Scene_palette_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_palette_select::draw(Image& dst) const { impl->draw(dst); }
