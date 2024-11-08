#include <cassert>
#include "scene-palette-select.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/palette.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/core/palette.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-game.hpp"
#include "util/file/archive.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "host/command.hpp"

struct Scene_palette_select::Impl {
  Unique<Menu> menu {};
  Shared<Sprite> test_image {};
  Strs m_palette_files {};
  std::size_t m_cur_palette_idx {};

  inline Impl() {
    init_menu();

    test_image = hpw::store_sprite->find("resource/image/other/palette test.png");
    assert(test_image);

    // загрузить имена всех файлов с палитрой
    auto file_list = hpw::archive->get_all_names(false);
    // фильтр списка
    std::erase_if(file_list, [](cr<Str> src) {
      return src.find("resource/image/palettes/") == Str::npos; });
    sort_by_color(file_list);
    m_palette_files = file_list;

    if ( !m_palette_files.empty()) {
      update_cur_palette_idx();
      // сразу применить палитру, как вошли в это окно
      hpw::init_palette_from_archive(cur_palette_file());
    }
  } // impl

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    return_if (graphic::current_palette_file.empty());

    // отобразить только имя файла палитры
    cauto palette_name = get_filename( cur_palette_file() );
    graphic::font->draw(dst, Vec(50, 25),
      get_locale_str("scene.palette_select.cur_file") + U" : " +
      sconv<utf32>(palette_name));

    menu->draw(dst);
    draw_palette(dst, Vec(50, 120));
    draw_test_image(dst, Vec(50, 150));
  }

  // докрутить индекс до текущего выбранного файла
  inline void update_cur_palette_idx() {
    std::size_t idx = 0;
    if ( !graphic::current_palette_file.empty()) {
      for (crauto fname: m_palette_files) {
        if (graphic::current_palette_file == fname) {
          m_cur_palette_idx = idx;
          break;
        }
        ++idx;
      }
    }
  }

  inline Str cur_palette_file() const
    { return m_palette_files.at(m_cur_palette_idx); }

  inline void init_menu() {
    init_unique<Text_menu>( menu,
      Menu_items {
        new_shared<Menu_text_item>(get_locale_str("scene.palette_select.next"), [this]{
          if ( !m_palette_files.empty()) {
            ++m_cur_palette_idx;
            if (m_cur_palette_idx >= m_palette_files.size())
              m_cur_palette_idx = 0;
            assert(hpw::init_palette_from_archive);
            hpw::init_palette_from_archive(cur_palette_file());
          }
        }),

        new_shared<Menu_text_item>(get_locale_str("scene.palette_select.prev"), [this]{
          if ( !m_palette_files.empty()) {
            if (m_cur_palette_idx == 0)
              m_cur_palette_idx = m_palette_files.size() - 1;
            else
              --m_cur_palette_idx;
            assert(hpw::init_palette_from_archive);
            hpw::init_palette_from_archive(cur_palette_file());
          }
        }),

        new_shared<Menu_text_item>(get_locale_str("common.reset"), [this]{ 
          graphic::current_palette_file = Str{graphic::DEFAULT_PALETTE_FILE};
          update_cur_palette_idx();
          hpw::init_palette_from_archive(cur_palette_file());
        }),
        
        new_shared<Menu_text_item>(get_locale_str("common.back"),
          []{ hpw::scene_mgr->back(); }),
      },

      Vec{50, 50}
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

  inline void draw_test_image(Image& dst, const Vec pos) const {
    crauto test = *test_image;
    insert(dst, test, pos);
    // рамка вокруг картинки
    draw_rect(dst, Rect(pos - Vec(1, 1), Vec(test.X(), test.Y()) + Vec(2, 2)), Pal8::black);
    draw_rect(dst, Rect(pos - Vec(2, 2), Vec(test.X(), test.Y()) + Vec(4, 4)), Pal8::white);
  }

  // сортирует список палитр по цветам
  inline static void sort_by_color(Strs& dst_list) {
    assert(hpw::archive);
    cauto comp = [](cr<Str> a, cr<Str> b) {
      cauto a_colors = colors_from_pal24(hpw::archive->get_file(a));
      cauto b_colors = colors_from_pal24(hpw::archive->get_file(b));
      cauto a_score = pal24_score(a_colors);
      cauto b_score = pal24_score(b_colors);
      return a_score > b_score;
    };
    std::sort(dst_list.begin(), dst_list.end(), comp);
  }

  // создаёт код для сравнения цветовых палитр
  inline static uint pal24_score(cr<Vector<Rgb24>> pal24) {
    assert(pal24.size() >= 256);
    // взять часть серого сектора и вычислить среднюю цветность
    const std::size_t ED = Pal8::gray_end * 0.9;
    const std::size_t ST = Pal8::gray_end * 0.4;
    cauto LEN = ED - ST;
    real avg_hue {};
    for (std::size_t i = ST; i < ED; ++i) {
      cauto rgb24 = pal24.at(i);
      avg_hue += rgb24_to_hue(rgb24);
    }
    avg_hue /= LEN;

    // средняя яркость конца палитры тоже повлияет на результат
    cauto a = pal24.at(ED * 0.85);
    cauto b = pal24.at(Pal8::gray_end);
    cauto a_max = std::max(std::max(a.r, a.g), a.b);
    cauto b_max = std::max(std::max(b.r, b.g), b.b);
    cauto ratio = (a_max + b_max) / 2.f;

    return avg_hue * 1'000 + ratio * 100'000;
  }

  inline static real rgb24_to_hue(cr<Rgb24> src) {
    const real r = src.r / 255.f;
    const real g = src.g / 255.f;
    const real b = src.b / 255.f;
    real min = r < g ? r : g;
    min = min < b ? min : b;
    real max = r > g ? r : g;
    max = max > b ? max : b;
    cauto delta = max - min;

    // undefined, maybe nan?
    return_if (delta < 0.00001f, 0);
    return_if(max == 0, 0);

    real hue {};
    if(r >= max) // > is bogus, just keeps compilor happy
      return (g - b) / delta; // between yellow & magenta
    elif (g >= max)
      hue = 2.0 + (b - r) / delta; // between cyan & yellow
    else
      hue = 4.0 + (r - g) / delta; // between magenta & cyan
    hue *= 60.0; // degrees

    if(hue < 0)
      hue += 360;
    return hue;
  }
}; // impl

Scene_palette_select::Scene_palette_select(): impl {new_unique<Impl>()} {}
Scene_palette_select::~Scene_palette_select() {}
void Scene_palette_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_palette_select::draw(Image& dst) const { impl->draw(dst); }
