#include <cassert>
#include <functional>
#include "scene-effect-plugin-select.hpp"
#include "scene-manager.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-effect-plugin.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/scene/scene-game.hpp"
#include "game/util/game-util.hpp"
#include "game/game-canvas.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"

using registrate_param_f32_ft = void (*)(Cstr, Cstr, float*, const float, const float, const float);
using registrate_param_i32_ft = void (*)(Cstr, Cstr, int*, const int, const int, const int);

/// Передаёт данные в эффект
struct Context {
  Pal8* dst {};
  uint16_t w {};
  uint16_t h {};
  registrate_param_f32_ft registrate_param_f32 {};
  registrate_param_i32_ft registrate_param_i32 {};
};

/// для получения данных с эффекта
struct Result {
  uint8_t version {};
  Cstr full_name {};
  Cstr description {};
  Cstr error {};
  bool init_succsess {};
};

using init_ft = void (const struct Context* context, struct Result* result);
using apply_ft = void (uint32_t state);
using finalize_ft = void (void);

// ----------- [!] ---------------
// вверх не перемещать
#include <DyLib/DyLib.hpp>
//#include <thirdparty/include/DyLib/DyLib.hpp>
// ----------- [!] ---------------

struct Scene_effect_plugin_select::Impl {
  Unique<Advanced_text_menu> menu {};
  Strs m_effects {}; /// список путей к файлам эффектов
  std::size_t selected_effect {};

  inline Impl() {
    init_menu();
    init_plugins();
  } // impl

  inline void update(double dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    menu->draw(dst);
    
    /*return_if (graphic::current_palette_file.empty());

    // отобразить только имя файла палитры
    auto palette_name = cur_palette_file();
    palette_name = std::filesystem::path(palette_name).stem().string();
    graphic::font->draw(dst, Vec(50, 25),
      get_locale_str("scene.palette_select.cur_file") + U" : " +
      sconv<utf32>(palette_name));

    menu->draw(dst);
    draw_palette(dst, Vec(50, 120));
    draw_test_image(dst, Vec(50, 150));*/
  }

  inline void init_menu() {
    menu = new_unique<Advanced_text_menu>(
      get_locale_str("scene.effect_plugin_select.title"),
      Menu_items {
        /*new_shared<Menu_text_item>(get_locale_str("scene.palette_select.next"), [this]{
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
        }),*/

        new_shared<Menu_text_item>(get_locale_str("common.back"),
          []{ hpw::scene_mgr->back(); }),
      },

      Rect(50, 50, 400, 300)
    );
  } // init_menu

  inline void init_plugins() {
    // загрузить пути к эффектам
    auto path = hpw::cur_dir + "plugin/effect/";
    conv_sep(path);
    m_effects = files_in_dir(path);

    // TODO set current
    selected_effect = 0;

    // TODO инициализация в другом месте
    hpw::shared_lib_loader = new_shared<DyLib>();
    hpw::shared_lib_loader->open( get_current_effect() );
    cauto init_f = hpw::shared_lib_loader->getFunction<init_ft>("init");
    cauto apply_f = hpw::shared_lib_loader->getFunction<apply_ft>("apply");
    cauto finalize_f = hpw::shared_lib_loader->getFunction<finalize_ft>("finalize");
    assert(init_f);
    assert(apply_f);
    assert(finalize_f);

    Context context;
    context.dst = graphic::canvas->data();
    context.w = graphic::canvas->X;
    context.h = graphic::canvas->Y;
    context.registrate_param_f32 = {}; // TODO
    context.registrate_param_i32 = {}; // TODO

    Result result;
    init_f(&context, &result);
    hpw_log("plugin fullname: " << result.full_name << "\n");
  }

  inline Str get_current_effect() const { return m_effects.at(selected_effect); }

}; // impl

Scene_effect_plugin_select::Scene_effect_plugin_select(): impl {new_unique<Impl>()} {}
Scene_effect_plugin_select::~Scene_effect_plugin_select() {}
void Scene_effect_plugin_select::update(double dt) { impl->update(dt); }
void Scene_effect_plugin_select::draw(Image& dst) const { impl->draw(dst); }
