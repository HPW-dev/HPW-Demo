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
#include "game/util/glass-ball.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/scene/scene-game.hpp"
#include "util/file/archive.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/math/timer.hpp"
#include "host/command.hpp"

constx Vec cube_sz(13, 13); // размер квадратика для теста оттенков

struct Scene_palette_select::Impl {
  Unique<Menu> _menu {};
  Shared<Sprite> _test_image {};
  Glass_ball _gb {};
  bool _show_gb {true};
  bool _hide_menu {};
  Timer _after_press {0.25};

  inline Impl() {
    init_menu();
    _test_image = hpw::sprites.find("resource/image/other/palette test.png");
    assert(_test_image);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
    if (_show_gb)
      _gb.update(dt);

    // если что-то нажали, меню обратно показываем
    if (_hide_menu && _after_press.update(dt) && is_any_key_pressed()) {
      _hide_menu = false;
    }
  }

  inline void draw(Image& dst) const {;
    draw_test_image(dst);

    if (_show_gb)
      _gb.draw(dst);

    if (!_hide_menu) {
      _menu->draw(dst);
      cauto pal_pos = Vec(_menu->rect().pos.x, _menu->rect().bottom() + 9);
      draw_palette(dst, pal_pos);
      draw_debug_colors(dst, pal_pos + Vec(0, 30));
    }
  }

  inline void init_menu() {
    Text_menu_config menu_config;
    menu_config.with_bg = true;

    init_unique<Text_menu>( _menu,
      Menu_items {
        get_palette_list(),
        get_test_image_list(),
        new_shared<Menu_bool_item>(get_locale_str("palette_select.show_gb"),
          [this]{ return _show_gb; }, [this](bool val){ _show_gb = val; } ),
        new_shared<Menu_text_item>(get_locale_str("common.hide_nemu"),
          [this]{ _hide_menu = true; }),
        new_shared<Menu_text_item>(get_locale_str("common.reset"), [this]{ 
          graphic::current_palette_file = Str{graphic::DEFAULT_PALETTE_FILE};
          hpw::init_palette_from_archive(graphic::current_palette_file);
          //graphic::cur_test_image_path = graphic::DEFAULT_TEST_IMAGE;
          init_menu();
        }),
        new_shared<Menu_text_item>(get_locale_str("common.back"),
          []{ hpw::scene_mgr.back(); }),
      },

      Vec{30, 25}, menu_config
    );
  } // init_menu

  inline void draw_palette(Image& dst, Vec pos) const {
    pos += Vec(2, 2);

    cauto pal_w = 256;
    cauto pal_h = 11;
    cfor (x, pal_w)
    cfor (y, pal_h)
      dst.set(x + pos.x, y + pos.y, Pal8(x), {});

    // рамка вокруг палитры
    draw_rect(dst, Rect(pos - Vec(1, 1), Vec(pal_w, pal_h) + Vec(2, 2)), Pal8::black);
    draw_rect(dst, Rect(pos - Vec(2, 2), Vec(pal_w, pal_h) + Vec(4, 4)), Pal8::white);
  }

  inline void draw_debug_color(Image& dst, Vec pos, Pal8 color, cr<utf32> title) const {
  #ifdef DEBUG
    draw_rect_filled(dst, Rect(pos - Vec(1, 1), cube_sz + Vec(2, 2)), Pal8::black);
    draw_rect_filled(dst, Rect(pos, cube_sz), color);
    graphic::font->draw(dst, pos + Vec(cube_sz.x + 4, 0), title, &blend_diff);
  #endif
  }

  inline void draw_debug_colors(Image& dst, Vec pos) const {
  #ifdef DEBUG
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 0), Pal8::black, U"black");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 1), Pal8::gray, U"gray");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 2), Pal8::gray_end, U"gray end");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 3), Pal8::red_start, U"red start");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 4), Pal8::red_mid, U"red mid");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 5), Pal8::red, U"red");
    draw_debug_color(dst, pos + Vec(0, (cube_sz.y + 4) * 6), Pal8::white, U"white");
  #endif
  }
}; // impl

Scene_palette_select::Scene_palette_select(): impl {new_unique<Impl>()} {}
Scene_palette_select::~Scene_palette_select() {}
void Scene_palette_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_palette_select::draw(Image& dst) const { impl->draw(dst); }
