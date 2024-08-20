#include <cassert>
#include "scene-palette-select.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
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
    m_palette_files = file_list;

    if ( !m_palette_files.empty()) {
      // докрутить индекс до текущего выбранного файла
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

}; // impl

Scene_palette_select::Scene_palette_select(): impl {new_unique<Impl>()} {}
Scene_palette_select::~Scene_palette_select() {}
void Scene_palette_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_palette_select::draw(Image& dst) const { impl->draw(dst); }
