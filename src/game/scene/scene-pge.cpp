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
#include "game/menu/item/int-item.hpp"
#include "game/menu/item/double-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/util/game-util.hpp"
#include "util/path.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"

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
    if (is_pressed_once(hpw::keycode::escape)) {
      save_pge_to_config();
      hpw::scene_mgr->back();
    }

    if (m_reinit_menu)
      init_menu();

    m_menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);

    if (!m_reinit_menu) // не показывать меню, когда его нужно перестроить
      m_menu->draw(dst);
  }

  inline void init_menu() {
    m_reinit_menu = false;

    Menu_items menu_items {
      new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.pge.selected"),
        [this] { 
          m_reinit_menu = true;
          if ( !m_effects.empty()) {
            save_pge_to_config();
            m_selected_effect = (m_selected_effect + 1) % m_effects.size();
            load_pge( get_current_effect() );
          }
        },
        []->utf32 {
          cauto cur_plug = sconv<utf32>(get_cur_pge_name());
          return cur_plug.empty() ? U"-" : cur_plug;
        },
        sconv<utf32>(get_cur_pge_description())
      )
    };

    // накидать опций от плагина
    cnauto params = get_pge_params();
    for (cnauto param: params) {
      assert(param);
      switch (param->type) {
        case Param_pge::Type::param_int: {
          cauto casted = cptr2ptr< CP<Param_pge_int> >(param.get());
          menu_items.push_back( new_shared<Menu_int_item>(
            sconv<utf32>(casted->title),
            [casted]->int { return *(casted->value); },
            [casted](const int val) { *(casted->value) = std::clamp(val, casted->min, casted->max); },
            casted->speed_step,
            sconv<utf32>(casted->description)
          ) );
          break;
        } // param_int
        
        case Param_pge::Type::param_real: {
          cauto casted = cptr2ptr< CP<Param_pge_real> >(param.get());
          menu_items.push_back( new_shared<Menu_double_item>(
            sconv<utf32>(casted->title),
            [casted]->double { return *(casted->value); },
            [casted](const double val) { *(casted->value) = std::clamp<double>(val, casted->min, casted->max); },
            casted->speed_step,
            sconv<utf32>(casted->description)
          ) );
          break;
        } // param_real

        case Param_pge::Type::param_bool: {
          cauto casted = cptr2ptr< CP<Param_pge_bool> >(param.get());
          menu_items.push_back( new_shared<Menu_bool_item>(
            sconv<utf32>(casted->title),
            [casted]->bool { return *(casted->value); },
            [casted](const bool val) { *(casted->value) = val; },
            sconv<utf32>(casted->description)
          ) );
          break;
        } // param_bool

        default:
        case Param_pge::Type::base:
          error("unregistered plugin parameter type");
          break;
      }
    } // for params
    
    menu_items.push_back(
      new_shared<Menu_text_item>(get_locale_str("scene.graphic_menu.pge.disable"),
        [this] {
          disable_pge();
          m_reinit_menu = true;
        }
      )
    );
    menu_items.push_back(
      new_shared<Menu_text_item>(get_locale_str("common.back"),
        [] {
          save_pge_to_config();
          hpw::scene_mgr->back();
        }
      )
    );
    m_menu = new_unique<Advanced_text_menu>(
      get_locale_str("scene.graphic_menu.pge.title"),
      menu_items, Rect(50, 50, 400, 300)
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
