#include <cassert>
#include <functional>
#include "scene-pge.hpp"
#include "scene-manager.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/pge.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/util/game-util.hpp"
#include "util/path.hpp"
#include "util/str-util.hpp"

struct Scene_pge::Impl {
  Unique<Advanced_text_menu> m_menu {};
  Strs m_effects {}; /// список путей к файлам эффектов
  std::size_t m_selected_effect {};
  bool m_reinit_menu {}; /// вызовет повторную инициализацию меню

  inline Impl() {
    init_menu();
    init_plugins();
  } // impl

  inline void update(double dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    if (m_reinit_menu)
      init_menu();
    m_menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    m_menu->draw(dst);
    
    /*return_if (graphic::current_palette_file.empty());

    // отобразить только имя файла палитры
    auto palette_name = cur_palette_file();
    palette_name = std::filesystem::path(palette_name).stem().string();
    graphic::font->draw(dst, Vec(50, 25),
      get_locale_str("scene.palette_select.cur_file") + U" : " +
      sconv<utf32>(palette_name));

    m_menu->draw(dst);
    draw_palette(dst, Vec(50, 120));
    draw_test_image(dst, Vec(50, 150));*/
  }

  inline void init_menu() {
    m_reinit_menu = false;

    m_menu = new_unique<Advanced_text_menu>(
      get_locale_str("scene.graphic_menu.pge.title"),
      Menu_items {
        new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.pge.selected"),
          [this] { 
            m_reinit_menu = true;
            if ( !m_effects.empty()) {
              m_selected_effect = (m_selected_effect + 1) % m_effects.size();
              load_pge( get_current_effect() );
            }
          },
          []->utf32 {
            cauto cur_plug = sconv<utf32>(get_cur_pge_name());
            return cur_plug.empty() ? U"-" : cur_plug;
          }
        ),
        new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.pge.disable"),
          [this] {
            disable_pge();
            m_reinit_menu = true;
          }
        ),
        new_shared<Menu_text_item>(get_locale_str("common.back"),
          [] {
            save_pge_to_config();
            hpw::scene_mgr->back();
          }
        ),
      },

      Rect(50, 50, 400, 300)
    );
  } // init_menu

  inline void init_plugins() {
    // загрузить пути к эффектам
    auto path = hpw::cur_dir + "plugin/effect/";
    conv_sep(path);
    m_effects = files_in_dir(path);
    return_if(m_effects.empty());
    m_effects.push_back({});
    std::swap(*m_effects.begin(), *(m_effects.end()-1));

    cauto pge_name = get_cur_pge_name();

    if (pge_name.empty()) {
      m_selected_effect = 0;
    } else {
      // докрутить индекс до выбранного эффекта
      for (uint idx = 0; auto effect: m_effects) {
        effect = get_filename(effect);
        if (effect == pge_name) {
          m_selected_effect = idx;
          break;
        }
        ++idx;
      }
    }

    load_pge( get_current_effect() );
  } // init_plugins

  inline Str get_current_effect() const { return m_effects.at(m_selected_effect); }
}; // impl

Scene_pge::Scene_pge(): impl {new_unique<Impl>()} {}
Scene_pge::~Scene_pge() {}
void Scene_pge::update(double dt) { impl->update(dt); }
void Scene_pge::draw(Image& dst) const { impl->draw(dst); }
