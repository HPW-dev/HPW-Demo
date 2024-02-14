#include <cassert>
#include "plugin/graphic-effect/hpw-plugin-effect.h"
#include "plugin-graphic-effect.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "graphic/image/image.hpp"
#include "game/game-canvas.hpp"

// ----------- [!] ---------------
// вверх не перемещать
#include <DyLib/DyLib.hpp>
// ----------- [!] ---------------

DyLib g_lib_loader {}; /// для кросплатформ загрузки либ
Vector<Shared<Param_pge>> g_pge_params {}; /// какие сейчас доступны настройки плагина
Str g_pge_path {}; /// текущий путь к плагину 
Str g_pge_name {}; /// текущее имя плагина

void load_pge(Str libname) {
  conv_sep(libname);
  try {
    std::cout << "загрузка плагина: " << libname << std::endl;
    g_lib_loader.open(libname);
    cauto init_f = g_lib_loader.getFunction<decltype(plugin_init)>("plugin_init");
    cauto apply_f = g_lib_loader.getFunction<decltype(plugin_apply)>("plugin_apply");
    cauto finalize_f = g_lib_loader.getFunction<decltype(plugin_finalize)>("plugin_finalize");
    iferror( !init_f, "не удалось получить функцию plugin_init");
    iferror( !apply_f, "не удалось получить функцию plugin_apply");
    iferror( !finalize_f, "не удалось получить функцию plugin_finalize");

    auto context = new_shared<context_t>();
    context->dst = ptr2ptr<pal8_t*>(graphic::canvas->data());
    context->w = graphic::canvas->X;
    context->h = graphic::canvas->Y;
    context->registrate_param_f32 = {}; // TODO
    context->registrate_param_i32 = {}; // TODO

    auto result = new_shared<result_t>();
    init_f(context.get(), result.get());
    iferror( result->version != 1, "несовпадение версий плагина и API");
    iferror( !result->init_succsess, result->error);
  } catch (CN<hpw::Error> err) {
    hpw_log("ошибка загрузки плагина: " << err.get_msg() << '\n');
  } catch (...) {
    hpw_log("неизвестная ошибка при загрузке плагина\n");
  }
} // load_pge

void apply_pge(Image& dst) {
  // TODO
}

void disable_pge() {
  // TODO
}

void load_pge_from_config() {
  // TODO
}

void save_pge_to_config() {
  // TODO
}

CN< Vector<Shared<Param_pge>> > get_pge_params() { return g_pge_params; }
CN<Str> get_cur_pge_path() { return g_pge_path; }
CN<Str> get_cur_pge_name() { return g_pge_name; }
