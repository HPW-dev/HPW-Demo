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
#include "game/util/blur-helper.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/palette-helper.hpp"
#include "game/util/locale.hpp"
#include "game/util/palette-helper.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/bgp/bgp.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/effect/blur.hpp"
#include "util/hpw-util.hpp"
#include "util/rnd-table.hpp"
#include "util/file/yaml.hpp"
#include "util/math/random.hpp"

#ifdef USE_NETPLAY
#include "game/netplay/scene/scene-netplay-menu.hpp"
#endif
#ifdef USE_TOOLS
#include "game/scene/scene-tools.hpp"
#endif

Scene_main_menu::Scene_main_menu() {
  init_menu();
  init_logo();
  if (hpw::menu_bgp_name.empty())
    randomize_menu_bgp();
}

Scene_main_menu::~Scene_main_menu() {}

void Scene_main_menu::update(const Delta_time dt) {
#ifdef DEBUG
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr.back();
#endif

  bg_state += dt;
  menu->update(dt);
  update_bg_order(dt);

  // чтобы перезагрузить локализацию строк
  if (hpw::scene_mgr.status().came_back)
    init_menu();
}

void Scene_main_menu::draw_bg(Image& dst) const noexcept {
  assert(hpw::menu_bgp);
  hpw::menu_bgp(dst, std::floor(pps(bg_state)));
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

void Scene_main_menu::draw_logo(Image& dst) const noexcept {
  insert<&blend_diff>(dst, *logo, logo_pos);
}

void Scene_main_menu::draw_wnd(Image& dst) const noexcept {
  // вырезаем часть фона и блюрим
  constexpr const Rect rect(120, 50, 270, 280);
  static Image for_blur(rect.size.x, rect.size.y);
  fast_cut_2(for_blur, dst, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);

  if (check_high_blur()) {
    blur_hq(for_blur, Image(for_blur), 5);
  } else {
    to_gray_accurate(for_blur, for_blur);
    static Image for_blur_tmp(rect.size.x, rect.size.y);
    for_blur_tmp = for_blur;
    boxblur_horizontal_fast(for_blur, for_blur_tmp, 5);
  }

  // мягкий контраст  
  apply_contrast(for_blur, 0.5);
  // затенение
  sub_brightness(for_blur, Pal8::from_real(0.33333));

  insert(dst, for_blur, rect.pos);
  // контур
  draw_rect<&blend_diff>(dst, rect, Pal8::white);
} // draw_wnd

void Scene_main_menu::draw(Image& dst) const noexcept {
  draw_bg(dst);
  draw_wnd(dst);
  draw_logo(dst);
  draw_text(dst);
}

void Scene_main_menu::init_menu() {
  init_unique<Text_menu>( menu,
    Menu_items {
      // старт
      new_shared<Menu_text_item>(get_locale_str("main_menu.start"),
        []{ hpw::scene_mgr.add(new_shared<Scene_difficulty>()); }),
        
      #ifdef USE_TOOLS
      // меню тестов и отладки
      new_shared<Menu_text_item>(get_locale_str("tools.title"), []{ hpw::scene_mgr.add(new_shared<Scene_tools>()); }),
      #endif

      #ifdef USE_NETPLAY
      // LAN
      new_shared<Menu_text_item>(get_locale_str("netplay.title"),
        []{ hpw::scene_mgr.add(new_shared<Scene_netplay_menu>()); }),
      #endif

      // смена языка
      new_shared<Menu_text_item>(hpw::locale_select_title,
        []{ hpw::scene_mgr.add(new_shared<Scene_locale_select>()); }),

      // сменить фон
      new_shared<Menu_text_item>(get_locale_str("main_menu.next_bg"), [this]{ next_bg(); }),
      
      get_palette_list(), // сменить палитру

      // инфа о разрабах TODO
      /*new_shared<Menu_text_item>(get_locale_str("main_menu.info"), []{
        hpw::scene_mgr.add(new_shared<Scene_info>());
      }),*/

      // опции
      new_shared<Menu_text_item>(get_locale_str("options.title"),
        []{ hpw::scene_mgr.add(new_shared<Scene_options>()); }),

      // выйти из игры
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
    }
  ); // init menu

  init_menu_sounds();
} // init_menu

void Scene_main_menu::next_bg() {
  init_logo();
  randomize_menu_bgp();
  change_bg_timer.reset();
}

void Scene_main_menu::cache_logo_names() {
  cauto config_file = load_res("resource/image/logo/list.yml");
  cauto config_yml = Yaml(config_file);
  m_logo_names = config_yml.get_v_str("logos");
}

void Scene_main_menu::draw_text(Image& dst) const noexcept {
  assert(graphic::font);

  // буффер текста меню
  constexpr Rect MENU_TXT_RECT (140, 200, 270, 280);
  static Image menu_txt;
  menu_txt.init(MENU_TXT_RECT.size.x, MENU_TXT_RECT.size.y, Pal8::black);
  // нарисовать надписи меню
  menu->draw(menu_txt);
  // буффер тени текста меню
  static Image menu_txt_shadows;
  menu_txt_shadows.init(menu_txt);
  apply_invert(menu_txt_shadows);
  // расширить контуры теней текста меню
  static Image menu_txt_shadows_expand_buf;
  menu_txt_shadows_expand_buf.init(menu_txt_shadows);
  expand_color_4_buf(menu_txt_shadows, menu_txt_shadows_expand_buf, Pal8::black);
  // наложение теней контуров текста меню на dst
  insert<&blend_min>(dst, menu_txt_shadows, MENU_TXT_RECT.pos);
  // отрисовка текста меню поверх теней на dst
  insert<&blend_max>(dst, menu_txt, MENU_TXT_RECT.pos);

  // версия игры
  cauto GAME_VER_TXT = prepare_game_ver();
  // буффер текста меню
  cauto GAME_VER_TXT_SZ = graphic::font->text_size(GAME_VER_TXT);
  const Rect GAME_VER_TXT_RECT (
    dst.X - GAME_VER_TXT_SZ.x - 7,
    dst.Y - GAME_VER_TXT_SZ.y - 4,
    GAME_VER_TXT_SZ.x + 6, GAME_VER_TXT_SZ.y + 6
  );
  static Image game_ver;
  game_ver.init(GAME_VER_TXT_RECT.size.x, GAME_VER_TXT_RECT.size.y, Pal8::black);
  // нарисовать надписи меню
  graphic::font->draw(game_ver, {3, 3}, GAME_VER_TXT);
  // буффер тени текста меню
  static Image game_ver_shadows;
  game_ver_shadows.init(game_ver);
  apply_invert(game_ver_shadows);
  // расширить контуры теней текста меню
  static Image game_ver_shadows_expand_buf;
  game_ver_shadows_expand_buf.init(game_ver_shadows);
  expand_color_8_buf(game_ver_shadows, game_ver_shadows_expand_buf, Pal8::black);
  game_ver_shadows_expand_buf = game_ver_shadows;
  expand_color_4_buf(game_ver_shadows, game_ver_shadows_expand_buf, Pal8::black);
  // наложение теней контуров текста меню на dst
  insert<&blend_min>(dst, game_ver_shadows, GAME_VER_TXT_RECT.pos);
  // отрисовка текста меню поверх теней на dst
  insert<&blend_max>(dst, game_ver, GAME_VER_TXT_RECT.pos);
}

Unique<Sprite> Scene_main_menu::prepare_logo(cr<Str> name) const {
  auto finded_sprite = hpw::sprites.find(name);
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

  if (!game_date.empty()) {
    game_ver += U" (";
    game_ver += game_date;
    game_ver += U')';
  }

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
  ret_if(!hpw::autoswith_bgp);
  
  // поменять фон возвращаясь из сцены
  const bool came_back = hpw::scene_mgr.status().came_back;
  // поменять фон через кнопку
  const bool fast_forward = is_pressed_once(hpw::keycode::fast_forward);
  // поменять фон по таймеру
  const bool bg_timer_ready = change_bg_timer.update(dt);

  if (came_back || fast_forward || bg_timer_ready)
    next_bg();
}
