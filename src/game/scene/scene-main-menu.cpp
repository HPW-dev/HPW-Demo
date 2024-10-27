#include <ctime>
#include "scene-main-menu.hpp"
#include "scene-game.hpp"
#include "scene-options.hpp"
#include "scene-loading.hpp"
#include "scene-difficulty.hpp"
#include "scene-locale.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/sounds.hpp"
#include "game/core/sprites.hpp"
#include "game/core/graphic.hpp"
#include "game/util/keybits.hpp"
#include "game/util/version.hpp"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/effect/blur.hpp"
#include "graphic/effect/bg-pattern.hpp"
#include "graphic/effect/bg-pattern-2.hpp"
#include "graphic/effect/bg-pattern-3.hpp"
#include "util/hpw-util.hpp"
#include "util/rnd-table.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"

void bg_copy_1(Image& dst, const int state);
void bg_copy_2(Image& dst, const int state);
void bg_copy_3(Image& dst, const int state);
void bg_copy_4(Image& dst, const int state);

void Scene_main_menu::init_bg() {
  sconst Rnd_table<decltype(bg_pattern_pf)> bg_patterns {{
  // Пак 1:
  #if 1
    &bgp_hpw_text_lines,
    &bgp_bit_1,
    &bgp_bit_2,
    &bgp_pinterest_1,
    &bgp_random_lines_1,
    &bgp_random_lines_2,
    &bgp_3d_atomar_cube,
    &bgp_circles,
    &bgp_circles_2,
    &bgp_circles_moire,
    &bgp_circles_moire_2,
    &bgp_red_circles_1,
    &bgp_red_circles_2,
    &bgp_pixel_font,
    &bgp_numbers,
    &bgp_numbers_alpha,
    &bgp_ipv4,
    &bgp_ipv4_2,
    &bgp_unicode,
    &bgp_unicode_red,
    &bgp_glsl_spheres,
    &bgp_clock,
    &bgp_clock_24,
    &bgp_graph,
    #ifndef ECOMEM
      &bgp_3d_rain_waves,
      &bgp_3d_waves,
      &bgp_3d_terrain,
      &bgp_3d_terrain_ps1,
      &bgp_3d_flat_stars,
      &bg_copy_1,
      &bg_copy_2,
      &bg_copy_3,
      &bg_copy_4,
      &bgp_rain_waves,
      &bgp_line_waves,
      &bgp_rotated_lines,
      &bgp_labyrinth_1,
      &bgp_labyrinth_2,
    #endif
  #endif
  // Пак 2:
  #if 1
    &bgp_self_code,
    &bgp_noise,
    &bgp_tile_corruption,
    &bgp_deep_circles,
    &bgp_deep_circles_red,
    &bgp_deep_lines,
    &bgp_deep_lines_2,
    &bgp_deep_lines_3,
    &bgp_deep_lines_red,
    &bgp_deep_lines_red_2,
    &bgp_deep_lines_red_3,
    &bgp_dither_wave,
    &bgp_dither_wave_2,
    &bgp_fast_lines,
    &bgp_fast_lines_red,
    &bgp_epilepsy,
    &bgp_red_gradient,
    &bgp_red_gradient_2,
    &bgp_spline_zoom,
    &bgp_spline,
    &bgp_bounding_lines,
    &bgp_bounding_repeated_circles,
    &bgp_repeated_rectangles,
    &bgp_bounding_circles,
    &bgp_skyline,
    &bgp_warabimochi,
    &bgp_circle_with_text,
    &bgp_red_circle_white,
    &bgp_red_circle_black,
    &bgp_tiles_1,
    &bgp_tiles_2,
    &bgp_tiles_3,
    &bgp_tiles_4,
    &bgp_tiles_4,
    &bgp_zoom_dst,
  #endif
  // Пак 3:
  #if 1
    &bgp_3d_sky,
    &bgp_perlin_noise,
    &bgp_liquid,
    &bgp_liquid_gray,
    &bgp_trajectory,
    &bgp_fading_grid,
    &bgp_fading_grid_red,
    &bgp_fading_grid_red_small,
    &bgp_fading_grid_dithered,
    &bgp_striped_spheres,
    &bgp_rotating_moire,
    &bgp_rotating_moire_more_lines,
    &bgp_rotating_moire_rotated,
    &bgp_rotating_moire_triple,
    &bgp_moire_lines,
    #ifndef ECOMEM
      &bgp_rand_cellular_simul,
      &bgp_rand_cellular_simul_x2,
      &bgp_rand_cellular_simul_x4,
    #endif
  #endif
  }}; // bg_patterns table

  bg_pattern_pf = bg_patterns.rnd_stable();
} // init_bg

Scene_main_menu::Scene_main_menu() {
  init_menu();
  init_logo();
  init_bg();
}

Scene_main_menu::~Scene_main_menu() {}

void Scene_main_menu::update(const Delta_time dt) {
#ifdef DEBUG
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
#endif

  bg_state += dt;
  menu->update(dt);
  update_bg_order(dt);

  // чтобы перезагрузить локализацию строк
  if (hpw::scene_mgr->status.came_back)
    init_menu();
}

void Scene_main_menu::draw_bg(Image& dst) const {
  bg_pattern_pf(dst, std::floor(pps(bg_state)));
}

void Scene_main_menu::init_logo() {
  // получить список логотипов из конфига
  std::call_once(m_logo_load_once,
    [this]{ cache_logo_names(); });
  assert(!m_logo_names.empty());

  // случайный выбор логотипа из списка
  cauto idx = rndu_fast(m_logo_names.size()-1);
  cauto logo_name = m_logo_names.at(idx);
  logo = prepare_logo(logo_name);

  // нарисовать логотип по центру меню
  const Vec logo_sz(logo->X(), logo->Y());
  const Vec menu_sz(graphic::canvas->X, 160);
  logo_pos = center_point(menu_sz, logo_sz);
  logo_pos.y += 50;
} // init_logo

void Scene_main_menu::draw_logo(Image& dst) const {
  insert<&blend_diff>(dst, *logo, logo_pos);
}

void Scene_main_menu::draw_wnd(Image& dst) const {
  // вырезаем часть фона и блюрим
  constexpr const Rect rect(120, 50, 270, 280);
  static Image for_blur(rect.size.x, rect.size.y);
  fast_cut_2(for_blur, dst, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);

  if (graphic::fast_blur) {
    to_gray_accurate(for_blur, for_blur);
    boxblur_horizontal_fast(for_blur, Image(for_blur), 5);
  } else {
    blur_hq(for_blur, Image(for_blur), 5);
  }

  // мягкий контраст  
  apply_contrast(for_blur, 0.5);
  // затенение
  sub_brightness(for_blur, Pal8::from_real(0.33333));

  insert(dst, for_blur, rect.pos);
  // контур
  draw_rect<&blend_diff>(dst, rect, Pal8::white);
} // draw_wnd

void Scene_main_menu::draw(Image& dst) const {
  draw_bg(dst);
  draw_wnd(dst);
  draw_logo(dst);
  draw_text(dst);
}

void Scene_main_menu::init_menu() {
  init_unique<Text_menu>( menu,
    Menu_items {
      // старт
      new_shared<Menu_text_item>(get_locale_str("scene.main_menu.start"),
        []{ hpw::scene_mgr->add(new_shared<Scene_difficulty>()); }),
      // смена языка
      new_shared<Menu_text_item>(hpw::locale_select_title,
        []{ hpw::scene_mgr->add(new_shared<Scene_locale_select>()); }),
      // сменить фон
      new_shared<Menu_text_item>(get_locale_str("scene.main_menu.next_bg"),
        [this]{ next_bg(); }),
      // сменить палитру
      new_shared<Menu_text_item>(get_locale_str("scene.main_menu.rnd_pal"),
        []{ set_random_palette(); }),
      // инфа о разрабах TODO
      /*new_shared<Menu_text_item>(get_locale_str("scene.main_menu.info"), []{
        hpw::scene_mgr->add(new_shared<Scene_info>());
      }),*/
      // опции
      new_shared<Menu_text_item>(get_locale_str("scene.options.name"),
        []{ hpw::scene_mgr->add(new_shared<Scene_options>()); }),
      // выйти из игры
      new_shared<Menu_text_item>(get_locale_str("common.exit"),
        []{ hpw::scene_mgr->back(); }),
    },
    Vec{140, 200}
  ); // init menu

  init_menu_sounds();
} // init_menu

void Scene_main_menu::next_bg() {
  init_logo();
  init_bg();
  change_bg_timer.reset();
}

void Scene_main_menu::cache_logo_names() {
  cauto config_file = hpw::archive->get_file("resource/image/logo/list.yml");
  cauto config_yml = Yaml(config_file);
  m_logo_names = config_yml.get_v_str("logos");
}

void bg_copy_1(Image& dst, const int state) {
  const Vec pos {
    rnd_fast(-1, 1),
    rnd_fast(-1, 1)
  };
  return_if(pos.is_zero());
  static Image buffer(dst.X, dst.Y);
  assert(buffer.size == dst.size);
  insert_fast(buffer, dst);
  insert<&blend_diff>(dst, buffer, pos);
}

void bg_copy_2(Image& dst, const int state) {
  const Vec pos {
    rnd_fast(-1, 1),
    rnd_fast(-1, 1)
  };
  return_if(pos.is_zero());
  static Image buffer(dst.X, dst.Y);
  assert(buffer.size == dst.size);
  insert_fast(buffer, dst);
  insert<&blend_xor>(dst, buffer, pos);
}

void bg_copy_3(Image& dst, const int state) {
  Vec pos {
    rnd_fast(-3, 3),
    rnd_fast(-3, 3)
  };
  pos += Vec(
    std::cos(scast<real>(state) * 0.01) * 3.0,
    std::sin(scast<real>(state) * 0.01) * 3.0
  );
  return_if(pos.is_zero());
  static Image buffer(dst.X, dst.Y);
  assert(buffer.size == dst.size);
  insert_fast(buffer, dst);
  insert(dst, buffer, pos);
}

void bg_copy_4(Image& dst, const int state) {
  cauto speed = rndr_fast(0, 4);
  const Vec pos(
    std::cos(scast<real>(state) * 0.01) * speed,
    std::sin(scast<real>(state) * 0.01) * speed
  );
  return_if(pos.is_zero());
  static Image buffer(dst.X, dst.Y);
  assert(buffer.size == dst.size);
  insert_fast(buffer, dst);
  insert(dst, buffer, pos);
}

void Scene_main_menu::draw_text(Image& dst) const {
  // нарисовать текст меню в маленькое окошко
  static Image menu_text_layer;
  static Image game_ver_layer;
  menu_text_layer.init(dst.X, dst.Y, Pal8::black);
  game_ver_layer.init(dst.X, dst.Y, Pal8::black);
  menu->draw(menu_text_layer);

  // показать версию игры  
  cauto game_ver = prepare_game_ver();
  const Vec game_ver_txt_pos {
    dst.X - graphic::font->text_width(game_ver) - 7,
    dst.Y - graphic::font->text_height(game_ver) - 4,
  };
  graphic::font->draw(game_ver_layer, game_ver_txt_pos, game_ver);

  // нарисовать тень от текста
  static Image shadow_layer;
  static Image shadow_layer_game_ver;
  shadow_layer = menu_text_layer;
  shadow_layer_game_ver = game_ver_layer;
  apply_invert(shadow_layer);
  apply_invert(shadow_layer_game_ver);
  expand_color_4(shadow_layer, Pal8::black);
  expand_color_8(shadow_layer_game_ver, Pal8::black);
  expand_color_4(shadow_layer_game_ver, Pal8::black);
  insert<&blend_min>(dst, shadow_layer_game_ver);
  insert<&blend_min>(dst, shadow_layer);

  // нарисовать текст поверх тени
  insert<&blend_max>(dst, menu_text_layer);
  insert<&blend_max>(dst, game_ver_layer);
} // draw_text

Unique<Sprite> Scene_main_menu::prepare_logo(cr<Str> name) const {
  auto finded_sprite = hpw::store_sprite->find(name);
  assert(finded_sprite && scast<bool>(*finded_sprite));
  auto logo = new_unique<Sprite>(*finded_sprite);

  // маленькие логотипы заресайзить
  if (logo->X() < 32 && logo->Y() < 16)
    zoom_x8(*logo);
  elif (logo->X() < 64 && logo->Y() < 32)
    zoom_x4(*logo);
  elif (logo->X() < 128 && logo->Y() < 64)
    zoom_x2(*logo);

  return logo;
}

utf32 Scene_main_menu::prepare_game_ver() const {
  auto game_date = sconv<utf32>( get_game_creation_date() );
  auto game_ver = sconv<utf32>( get_game_version() );
  if (game_ver.empty())
    game_ver = U"v???";

  game_ver += U" (";

  #ifdef DETAILED_LOG
    // добавить инфу по платформе и билду:
    #ifdef WINDOWS
      game_ver += U"Windows";
    #else
      game_ver += U"Linux";
    #endif

    #ifdef is_x64
      game_ver += U" x64";
    #else
      game_ver += U" x32";
    #endif

    #ifdef DEBUG
      game_ver += U" Debug";
    #else
      game_ver += U" Release";
    #endif

    #ifdef ECOMEM
      game_ver += U" Ecomem";
    #endif

    if (!game_date.empty())
      game_ver += U' ' + game_date;
  #else
    if (!game_date.empty())
      game_ver += game_date;
  #endif

  game_ver += U')';
  return game_ver;
}

void Scene_main_menu::init_menu_sounds() {
  // звук при выборе пункта меню
  menu->set_select_callback( [](Menu_item& item) {
    hpw::sound_mgr->play("sfx/UI/close.flac"); } );

  // звук при перемещении по пунктам меню
  menu->set_move_cursor_callback( [](Menu_item& item) {
    static const Vector<Str> names {
      "sfx/UI/Milpon guitar/1.flac",
      "sfx/UI/Milpon guitar/2.flac",
      "sfx/UI/Milpon guitar/3.flac",
      "sfx/UI/Milpon guitar/4.flac",
      "sfx/UI/Milpon guitar/5.flac",
      "sfx/UI/Milpon guitar/6.flac",
      "sfx/UI/Milpon guitar/7.flac",
      "sfx/UI/Milpon guitar/8.flac",
      "sfx/UI/Milpon guitar/9.flac",
      "sfx/UI/Milpon guitar/10.flac",
      "sfx/UI/Milpon guitar/11.flac",
    };
    hpw::sound_mgr->play(names.at(rndu_fast(names.size() - 1)), {}, {}, 0.3);
  } );
}

void Scene_main_menu::update_bg_order(const Delta_time dt) {
  // поменять фон возвращаясь из сцены
  const bool came_back = hpw::scene_mgr->status.came_back;
  // поменять фон через кнопку
  const bool fast_forward = is_pressed_once(hpw::keycode::fast_forward);
  // поменять фон по таймеру
  const bool bg_timer_ready = change_bg_timer.update(dt);

  if (came_back || fast_forward || bg_timer_ready)
    next_bg();
}
