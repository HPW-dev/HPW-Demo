#include <cassert>
#include <filesystem>
#include <functional>
#include <utility>
#include "pge.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"
#include "util/log.hpp"
#include "util/macro.hpp"
#include "util/error.hpp"
#include "util/file/yaml.hpp"
#include "util/platform.hpp"
#include "game/core/canvas.hpp"
#include "game/util/config.hpp"

// ----------- [!] ---------------
// вверх не перемещать
#include <DyLib/DyLib.hpp>
// ----------- [!] ---------------

Shared<DyLib> g_lib_loader {}; // для кросплатформ загрузки либ
Vector<Shared<Param_pge>> g_pge_params {}; // какие сейчас доступны настройки плагина
Str g_pge_path {}; // текущий путь к плагину 
Str g_pge_name {}; // текущее имя плагина
Str g_pge_description {}; // описание к плагину
Str g_pge_author {}; // имя автора эффекта
bool g_pge_loaded {false}; // флаг успешной загрузки плагина
std::function<decltype(plugin_init)> g_plugin_init {};
std::function<decltype(plugin_apply)> g_plugin_apply {};
std::function<decltype(plugin_finalize)> g_plugin_finalize {};
void registrate_param_f32(cstr_t, cstr_t, real_t*, const real_t, const real_t, const real_t);
void registrate_param_i32(cstr_t, cstr_t, std::int32_t*, const std::int32_t, const std::int32_t, const std::int32_t);
void registrate_param_bool(cstr_t, cstr_t, bool*);
void load_pge_params_only();

void load_pge(Str libname) {
  if (libname.empty()) {
    log_debug << "loading empty plugin (ignore)";
    disable_pge();
    return;
  }

  try {
    disable_pge();
    conv_sep(libname);
    libname = std::filesystem::weakly_canonical(libname).string();
    log_info << "загрузка плагина: " + libname;

    #ifdef WINDOWS
      init_shared<DyLib>(g_lib_loader, std::filesystem::path(libname).wstring().c_str());
    #else
      init_shared<DyLib>(g_lib_loader, libname.c_str());
    #endif
    g_plugin_init = g_lib_loader->getFunction<decltype(plugin_init)>("plugin_init");
    g_plugin_apply = g_lib_loader->getFunction<decltype(plugin_apply)>("plugin_apply");
    g_plugin_finalize = g_lib_loader->getFunction<decltype(plugin_finalize)>("plugin_finalize");
    iferror( !g_plugin_init, "не удалось получить функцию plugin_init");
    iferror( !g_plugin_apply, "не удалось получить функцию plugin_apply");
    iferror( !g_plugin_finalize, "не удалось получить функцию plugin_finalize");

    auto context = new_shared<context_t>();
    context->dst = ptr2ptr<pal8_t*>(graphic::canvas->data());
    context->w = graphic::canvas->X;
    context->h = graphic::canvas->Y;
    context->registrate_param_f32 = &registrate_param_f32;
    context->registrate_param_i32 = &registrate_param_i32;
    context->registrate_param_bool = &registrate_param_bool;

    auto result = new_shared<result_t>();
    g_plugin_init(context.get(), result.get());
    iferror( result->version != DEFAULT_EFFECT_API_VERSION,
      "несовпадение версий плагина и API");
    iferror( !result->init_succsess, result->error);
    g_pge_description = result->description;
    g_pge_author = result->author;
    g_pge_path = libname;
    g_pge_name = get_filename(g_pge_path);
    // попытаться найти настройки плагина в конфиге
    load_pge_params_only();
    log_info << "плагин " + g_pge_name + " успешно загружен.";
    g_pge_loaded = true;
  } catch (cr<hpw::Error> err) {
    log_error << "ошибка загрузки плагина: " + err.get_msg();
    disable_pge();
  } catch (...) {
    log_error << "неизвестная ошибка при загрузке плагина";
    disable_pge();
  }
} // load_pge

void apply_pge(const uint32_t state) {
  if (g_plugin_apply)
    g_plugin_apply(state);
}

void disable_pge() {
  log_debug << "отключение плагина " + get_cur_pge_name();
  g_pge_params.clear();
  g_pge_name.clear();
  g_pge_path.clear();
  g_pge_description.clear();
  g_pge_author.clear();
  if (g_plugin_finalize)
    g_plugin_finalize();
  g_plugin_finalize = {};
  g_plugin_apply = {};
  g_plugin_init = {};
  g_lib_loader = {};
  g_pge_loaded = false;
  save_pge_to_config();
}

// перенос значений с конфига в настройки плагина
void load_params(cr<Yaml> node) {
  rauto params = get_pge_params();
  for (uint id = 0; rauto param: params) {
    auto param_node = node["param_" + n2s(id)];
    ++id;
    param->load(param_node);
  }
}

// грузит только параметры для плагина
void load_pge_params_only() {
  assert(hpw::config);
  crauto config = *hpw::config;
  cauto plugin_node = config["plugin"];
  cauto graphic_node = plugin_node["graphic"];
  if (cauto effect_node = graphic_node[get_cur_pge_name()]; effect_node.check())
    load_params(effect_node);
}

void load_pge_from_config() {
  assert(hpw::config);
  crauto config = *hpw::config;
  cauto plugin_node = config["plugin"];
  cauto graphic_node = plugin_node["graphic"];
  cauto selected = graphic_node.get_str("selected");
  if (cauto effect_node = graphic_node[selected]; effect_node.check()) {
    cauto path = effect_node.get_str("path");
    load_pge(path);
    load_params(effect_node);
  }
}

void save_pge_to_config() {
  assert(hpw::config);
  auto& config = *hpw::config;
  auto plugin_node = config.make_node_if_not_exist("plugin");

  cauto plugin_name = get_cur_pge_name();
  auto graphic_node = plugin_node.make_node_if_not_exist("graphic");
  graphic_node.set_str("selected", plugin_name);
  if (plugin_name.empty()) {
    save_config();
    return;
  }

  auto effect_node = graphic_node.make_node_if_not_exist(plugin_name);
  effect_node.set_str("path", get_cur_pge_path());
  // сейв текущих настроек плагина
  crauto params = get_pge_params();
  for (uint id = 0; crauto param: params) {
    auto param_node = effect_node.make_node("param_" + n2s(id));
    ++id;
    assert(param);
    param->save(param_node);
  }

  save_config(); // сохраняет корневой файл конфига
} // save_pge_to_config

template <class T, class Param_type>
void registrate_param(cstr_t title, cstr_t desc, T* val,
const T speedstep, const T min, const T max) {
  auto param = new_shared<Param_type>();
  param->max = max;
  param->min = min;
  param->description = desc;
  param->title = title;
  param->value = val;
  param->speed_step = speedstep;
  iferror(speedstep <= 0, "speedstep не должен быть меньше 1");
  iferror(min >= max, "min не должен быть больше max");
  iferror( !val, "неправильный адрес для value");
  iferror(Str(title).empty(), "параметру нужно задать имя");
  g_pge_params.push_back( std::move(param) );
} // registrate_param

void registrate_param_f32(cstr_t title, cstr_t desc, real_t* val,
const real_t speedstep, const real_t min, const real_t max)
{ registrate_param<real_t, Param_pge_real>(title, desc, val, speedstep, min, max); }

void registrate_param_i32(cstr_t title, cstr_t desc, std::int32_t* val,
const std::int32_t speedstep, const std::int32_t min, const std::int32_t max)
{ registrate_param<std::int32_t, Param_pge_int>(title, desc, val, speedstep, min, max); }

void registrate_param_bool(cstr_t title, cstr_t desc, bool* val) {
  auto param = new_shared<Param_pge_bool>();
  param->description = desc;
  param->title = title;
  param->value = val;
  iferror( !val, "неправильный адрес для value");
  iferror(Str(title).empty(), "параметру нужно задать имя");
  g_pge_params.push_back( std::move(param) );
}

cr< Vector<Shared<Param_pge>> > get_pge_params() { return g_pge_params; }
cr<Str> get_cur_pge_path() { return g_pge_path; }
cr<Str> get_cur_pge_name() { return g_pge_name; }
cr<Str> get_cur_pge_description() { return g_pge_description; }
cr<Str> get_cur_pge_author() { return g_pge_author; }

void Param_pge::save(Yaml& dst) const {
  dst.set_str("title", title);
  dst.set_str("description", description);
  dst.set_int("type", scast<int>(type));
}

void Param_pge::load(cr<Yaml> dst) {
  title = dst.get_str("title");
  description = dst.get_str("description");
}

void Param_pge_int::save(Yaml& dst) const {
  Param_pge::save(dst);
  assert(value);
  dst.set_int("value", *value);
  dst.set_int("min", min);
  dst.set_int("max", max);
  dst.set_int("speed_step", speed_step);
}

void Param_pge_int::load(cr<Yaml> dst) {
  cauto loaded_type = scast<Param_pge::Type>(dst.get_int("type"));
  iferror(type != loaded_type, "type != loaded_type");
  assert(value);
  *value = dst.get_int("value");
  min = dst.get_int("min");
  max = dst.get_int("max");
  speed_step = dst.get_int("speed_step");
}

void Param_pge_real::save(Yaml& dst) const {
  Param_pge::save(dst);
  assert(value);
  dst.set_real("value", *value);
  dst.set_real("min", min);
  dst.set_real("max", max);
  dst.set_real("speed_step", speed_step);
}

void Param_pge_real::load(cr<Yaml> dst) {
  cauto loaded_type = scast<Param_pge::Type>(dst.get_int("type"));
  iferror(type != loaded_type, "type != loaded_type");
  assert(value);
  *value = dst.get_real("value");
  min = dst.get_real("min");
  max = dst.get_real("max");
  speed_step = dst.get_real("speed_step");
}

void Param_pge_bool::save(Yaml& dst) const {
  Param_pge::save(dst);
  assert(value);
  dst.set_bool("value", *value);
}

void Param_pge_bool::load(cr<Yaml> dst) {
  cauto loaded_type = scast<Param_pge::Type>(dst.get_int("type"));
  iferror(type != loaded_type, "type != loaded_type");
  assert(value);
  *value = dst.get_bool("value");
}

bool pge_loaded() { return g_pge_loaded; }
