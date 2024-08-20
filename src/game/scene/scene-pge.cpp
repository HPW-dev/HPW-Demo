#include <cassert>
#include <string>
#include <filesystem>
#include <utility>
#include <functional>
#include "scene-game.hpp"
#include "scene-pge.hpp"
#include "scene-mgr.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/pge.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/item/int-item.hpp"
#include "game/menu/item/double-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/util/game-util.hpp"
#include "util/path.hpp"
#include "util/math/vec-util.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"

struct Scene_pge::Impl {
  Unique<Advanced_text_menu> m_menu {};
  Strs m_effects {}; // список путей к файлам эффектов
  std::size_t m_selected_effect {};
  bool m_reinit_menu {}; // вызовет повторную инициализацию меню
  Image m_ball {}; // скачет по экрану и нужен для оценки эффекта
  Vec m_ball_pos {};
  constx real BALL_SPEED {2_pps};
  Vec m_ball_vel {BALL_SPEED, BALL_SPEED};

  inline Impl() {
    init_menu();
    init_plugins();
    init_ball();
  } // impl

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) {
      save_pge_to_config();
      hpw::scene_mgr->back();
    }

    if (m_reinit_menu)
      init_menu();

    m_menu->update(dt);
    update_ball(dt);
  }

  inline void draw(Image& dst) const {
    if (!m_reinit_menu) // не показывать меню, когда его нужно перестроить
      m_menu->draw(dst);
    draw_ball(dst);
  }

  inline void init_menu() {
    m_reinit_menu = false;

    Menu_items menu_items {
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
        },
        sconv<utf32>(get_cur_pge_description())
      )
    };

    // отобразить автора эффекта
    cauto author = get_cur_pge_author();
    if ( !author.empty()) {
      menu_items.push_back (
        new_shared<Menu_text_item> (
          get_locale_str("scene.graphic_menu.pge.author"),
          []{}, []->utf32 { return sconv<utf32>( get_cur_pge_author() ); }
        )
      );
    }

    // накидать опций от плагина
    crauto params = get_pge_params();
    for (crauto param: params) {
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
    
    menu_items.push_back( new_shared<Menu_text_item>(
      get_locale_str("common.back"), [] {
        save_pge_to_config();
        hpw::scene_mgr->back();
      } ) );
    menu_items.push_back( new_shared<Menu_text_item>(
      get_locale_str("scene.graphic_menu.pge.disable"), [this] {
        disable_pge();
        m_reinit_menu = true;
      } ) );

    init_unique( m_menu,
      get_locale_str("scene.graphic_menu.pge.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height}
    );
  } // init_menu

  inline void init_plugins() {
    // загрузить пути к эффектам
    auto path = hpw::cur_dir + "plugin/effect/";
    conv_sep(path);
    m_effects = files_in_dir(path);
    return_if(m_effects.empty());
    // оставить только .so/.dll имена
    std::erase_if(m_effects, [](CN<Str> fname)->bool {
      cauto ext = std::filesystem::path(fname).extension().string();
      return !(ext == ".so" || ext == ".dll"); // допустимые форматы для плагина
    });
    // чтобы в списке можно было выбирать пустой плагин
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

      if ( !pge_loaded()) // не грузить плагин, если уже загружен
        load_pge( get_current_effect() );
    }
  } // init_plugins

  inline Str get_current_effect() const { return m_effects.at(m_selected_effect); }

  inline void init_ball() {
    constexpr int sz = 64;
    m_ball.init(sz, sz, Pal8::black);
    cfor (y, sz)
    cfor (x, sz) {
      real luma = distance({sz/2, sz/2}, {x, y});
      luma /= sz/2;
      luma = 1.0 - luma;
      m_ball(x, y) = Pal8::from_real(luma);
    }
    graphic::font->draw(m_ball, {20, 30}, U"Test", &blend_diff);
  }

  inline void draw_ball(Image& dst) const {
    insert<&blend_diff>(dst, m_ball, m_ball_pos);
  }

  inline void update_ball(const Delta_time dt) {
    m_ball_pos.x += m_ball_vel.x * dt;
    m_ball_pos.y += m_ball_vel.y * dt;
    if (m_ball_pos.x >= graphic::width - m_ball.X) {
      m_ball_pos.x = graphic::width - m_ball.X - 1;
      m_ball_vel.x *= -1;
    }
    if (m_ball_pos.y >= graphic::height - m_ball.Y) {
      m_ball_pos.y = graphic::height - m_ball.Y - 1;
      m_ball_vel.y *= -1;
    }
    if (m_ball_pos.x < 0) {
      m_ball_pos.x = 0;
      m_ball_vel.x *= -1;
    }
    if (m_ball_pos.y < 0) {
      m_ball_pos.y = 0;
      m_ball_vel.y *= -1;
    }
  } // update_ball
}; // impl

Scene_pge::Scene_pge(): impl {new_unique<Impl>()} {}
Scene_pge::~Scene_pge() {}
void Scene_pge::update(const Delta_time dt) { impl->update(dt); }
void Scene_pge::draw(Image& dst) const { impl->draw(dst); }
