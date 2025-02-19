#define YAML_CPP_API
#include <yaml-cpp/yaml.h>
#include <format>
#include <codecvt>
#include <locale>
#include <cassert>
#include <utility>
#include <fstream>
#include <filesystem>
#include "yaml.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/file.hpp"

// Внутреннаяя реализация для Yaml
struct Yaml::Impl {
  Yaml& _master;
  YAML::Node root {};

  inline void get_kv_table_helper(vkv_t &v_kv, cr<Str> key, cr<decltype(root)> v_node) const {
    if (v_node.size() == 0) {
      v_kv.push_back(kv_t{ .key=str_tolower(key), .value=v_node.as<Str>() });
    } else {
      for (auto node: v_node)
        get_kv_table_helper(v_kv, key + "." + node.first.as<Str>(), node.second);
    }
  }

  inline void get_kv32_table_helper(vkvu32_t &v_kv32, cr<Str> key, cr<decltype(root)> v_node) const {
    if (v_node.size() == 0) {
      using utf8_to_utf32 = std::codecvt_utf8<char32_t>;
      std::wstring_convert<utf8_to_utf32, char32_t> conv;
      v_kv32.emplace_back(kvu32_t {
        .key = str_tolower(key),
        .str = conv.from_bytes(v_node.as<Str>())
      } );
    } else {
      for (auto node: v_node)
        get_kv32_table_helper(v_kv32, key + "." + node.first.as<Str>(), node.second);
    }
  } // get_kv32_table_helper

  template <class T>
  inline void set_v(cr<Str> name, cr<Vector<T>> val) {
    cfor (i, val.size())
      root[name][i] = val[i];
  }

  // helper for get value from YAML
  template <class T>
  inline T _get(cr<Str> name, cr<T> default_val) const {
    try {
      return root[name].as<T>();
    } catch (...) {
      log_debug << std::format("WARNING: Yaml._get: not finded \"{}\" in file \"{}\". Loaded default: {}",
        name, _master.get_path(), default_val);
      return default_val;
    }
  } // _get

  // версия для utf8 строк
  inline utf8 _get_utf8(cr<Str> name, cr<utf8> default_val) const {
    try {
      return sconv<utf8>(root[name].as<Str>());
    } catch (...) {
      log_warning << "WARNING: Yaml._get: not finded \"" + name
        + "\" in file \"" + _master.get_path()
        + "\". Loaded default str";
      return default_val;
    }
  } // _get utf8

  template <class T>
  inline Vector<T> _get_v(cr<Str> name, cr<Vector<T>> default_val) const {
    try {
      Vector<T> ret;
      auto items = root[name];
      iferror(items.size() == 0, "нет элементов с таким тегом");

      for (crauto str: items)
        ret.emplace_back(str.as<T>());
      return ret;
    } catch (...) {
      log_debug << "Yaml._get_v: not finded \"" + name
        + "\" in file \"" + _master.get_path()
        + "\". Loaded default value";
    }
    return default_val;
  } // _get_v

  inline Impl(Yaml& master) noexcept: _master{master} {}
  inline ~Impl() = default;
  
  inline Impl(Yaml& master, cr<Impl> other) noexcept: _master{master}, root{other.root} {}

  inline Impl(Yaml& master, Impl&& other) noexcept: _master{master}, root{std::move(other.root)} {}

  inline Impl(Yaml& master, Str fname, bool make_if_not_exist)
  : _master{master} {
    conv_sep(fname);
    fname = std::filesystem::weakly_canonical(fname).string();
    _master.set_path(fname);
    log_debug << "Yaml: loading \"" + fname + "\"";

    try {
      std::ifstream file(std::filesystem::path(fname), std::ios_base::binary);
      iferror (!file || !file.is_open(), "не удалось загрузить YAML файл \"" << fname << "\"");
      root = YAML::Load(file);
    } catch (...) { // если файла нет, то создать его с нуля
      if (make_if_not_exist) {
        log_warning << "файл \"" + fname + "\" отсутствует. Пересоздание";
        std::ofstream file(std::filesystem::path(fname), std::ios_base::binary);
        file.close();
      }

      std::ifstream file(std::filesystem::path(fname), std::ios_base::binary);
      iferror (!file || !file.is_open(), "не удалось загрузить YAML файл \"" << fname << "\"");
      root = YAML::Load(file);
    }
  } // c-tor(fname, make_if_not_exist)

  inline Impl(Yaml& master, cr<File> file): _master{master} {
    _master.set_path(file.get_path());
    log_debug << "Yaml: loading from memory \"" + file.get_path() + "\"";
    root = YAML::Load({file.data.begin(), file.data.end()});
  }

  inline Impl(Yaml& master, cr<Yaml> other) noexcept: _master{master} {
    if (std::addressof(_master) != std::addressof(other))
      root = other.impl->root;
  }

  inline Impl(Yaml& master, Yaml&& other) noexcept: _master{master} {
    if (std::addressof(_master) != std::addressof(other))
      root = std::move(other.impl->root);
  }

  inline Impl(Yaml& master, cr<YAML::Node> node, cr<Str> new_path)
  : _master{master}
  , root(node)
  { _master.set_path(new_path); }

  inline Impl& operator = (cr<Yaml> other) noexcept {
    if (std::addressof(_master) != std::addressof(other))
      root = other.impl->root;
    return *this;
  }

  inline Impl& operator = (Yaml&& other) noexcept {
    if (std::addressof(_master) != std::addressof(other))
      root = std::move(other.impl->root);
    return *this;
  }

  inline void save(Str fname) const {
    assert( !fname.empty());
    conv_sep(fname);
    log_debug << "save to file \"" + fname + "\"";
    std::ofstream file(std::filesystem::path(fname), std::ios_base::trunc); // удалить старый файл
    YAML::Emitter emt;
    emt << root;
    file << emt.c_str();
  }

  inline void clear() { root = {}; }

  inline Yaml make_node(cr<Str> name) {
    root[name] = YAML::Node(YAML::NodeType::Null);
    return Impl(_master, root[name], _master.get_path());
  }

  inline Yaml make_node_if_not_exist(cr<Str> name) {
    if (auto node = Impl(_master, root[name], _master.get_path()); node.check())
      return node;
    return make_node(name);
  }

  inline void delete_node(cr<Str> name) { root.remove(name); }

  inline void set_int(cr<Str> name, int val) { root[name] = val; }
  inline void set_real(cr<Str> name, real val) { root[name] = val; }
  inline void set_bool(cr<Str> name, bool val) { root[name] = val; }
  inline void set_str(cr<Str> name, cr<Str> val) { root[name] = val; }
  inline void set_utf8(cr<Str> name, cr<utf8> val) { root[name] = sconv<Str>(val); }
  inline void set_v_int(cr<Str> name, cr<Vector<int>> val) { set_v(name, val); }
  inline void set_v_str(cr<Str> name, cr<Vector<Str>> val) { set_v(name, val); }
  inline void set_v_real(cr<Str> name, cr<Vector<real>> val) { set_v(name, val); }

  inline Yaml operator[] (cr<Str> name) const {
    try {
      auto node = root[name];
      iferror(!node, "!node");
      // путь в ресурсе нельзя трогать, он должен указывать на рутовую ноду
      return Impl(_master, node, _master.get_path());
    }  catch(...) {
      log_debug << "node \"" + name + "\" not finded in yaml \"" + _master.get_path() + "\"";
    }

    Yaml ret;
    ret.set_path(_master.get_path() + "->" + name);
    ret.set_generated(true);
    return ret;
  }

  inline Str get_str(cr<Str> name, cr<Str> def) const { return _get(name, def); }
  inline int get_int(cr<Str> name, int def) const { return _get(name, def); }
  inline real get_real(cr<Str> name, real def) const { return _get(name, def); }
  inline bool get_bool(cr<Str> name, bool def) const { return _get(name, def); }
  inline utf8 get_utf8(cr<Str> name, cr<utf8> def) const  { return _get_utf8(name, def); }
  inline Vector<Str> get_v_str(cr<Str> name, cr<Vector<Str>> def) const { return _get_v(name, def); }
  inline Vector<int> get_v_int(cr<Str> name, cr<Vector<int>> def) const { return _get_v(name, def); }
  inline Vector<real> get_v_real(cr<Str> name, cr<Vector<real>> def) const { return _get_v(name, def); }

  inline Yaml::vkv_t get_kv_table() const {
    vkv_t v_kv;
    for (auto node: root)
      get_kv_table_helper(v_kv, node.first.as<Str>(), node.second);
    return v_kv;
  }

  inline Yaml::vkvu32_t get_kvu32_table() const {
    vkvu32_t ret;
    for (auto node: root)
      get_kv32_table_helper(ret, node.first.as<Str>(), node.second);
    return ret;
  }

  inline Strs root_tags() const {
    Strs ret;
    for (auto node: root)
      ret.push_back(node.first.as<Str>());
    return ret;
  }

  inline bool check() const { return root && root.size(); }

  inline Vector<Yaml> items() const {
    Vector<Yaml> ret;
    for (crauto node: root)
      ret.emplace_back( Impl(_master, YAML::Node(node), _master.get_path()) );
    return ret;
  }
}; // Impl

Yaml::Yaml(Str fname, bool make_if_not_exist): impl {new_unique<Impl>(*this, fname, make_if_not_exist)} {}

Yaml::Yaml(cr<File> file): impl {new_unique<Impl>(*this, file)} {}

Yaml::Yaml(Yaml&& other) noexcept: impl {new_unique<Impl>(*this, other)}
  { Resource::operator=(std::move(other)); }

Yaml::Yaml() noexcept: impl {new_unique<Impl>(*this)} {}

Yaml::Yaml(cr<Yaml> other): impl {new_unique<Impl>(*this, other)}
  { Resource::operator=(other); }

Yaml::Yaml(cr<Impl> new_impl): impl {new_unique<Impl>(*this, new_impl)}
  { Resource::operator=(new_impl._master); }

// деструктор должен бытьв .cpp файле, а то не выйдет запихнуть реализаццию в unique_ptr
Yaml::~Yaml() {}

Yaml& Yaml::operator = (cr<Yaml> other) noexcept {
  if (this != std::addressof(other)) {
    impl->operator =(other);
    Resource::operator=(other);
  }
  return *this;
}

Yaml& Yaml::operator = (Yaml&& other) noexcept {
  if (this != std::addressof(other)) {
    impl->operator =(std::move(other));
    Resource::operator=(std::move(other));
  }
  return *this;
}

void Yaml::save(Str fname) const { impl->save(fname); }
void Yaml::clear() { impl->clear(); }
Yaml Yaml::make_node(cr<Str> name) { return impl->make_node(name); }
Yaml Yaml::make_node_if_not_exist(cr<Str> name) { return impl->make_node_if_not_exist(name); }
void Yaml::delete_node(cr<Str> name) { impl->delete_node(name); }
void Yaml::set_int(cr<Str> name, int val) { impl->set_int(name, val); }
void Yaml::set_real(cr<Str> name, real val) { impl->set_real(name, val); }
void Yaml::set_bool(cr<Str> name, bool val) { impl->set_bool(name, val); }
void Yaml::set_str(cr<Str> name, cr<Str> val) { impl->set_str(name, val); }
void Yaml::set_utf8(cr<Str> name, cr<utf8> val) { impl->set_utf8(name, val); }
void Yaml::set_v_int(cr<Str> name, cr<Vector<int>> val) { impl->set_v_int(name, val); }
void Yaml::set_v_str(cr<Str> name, cr<Vector<Str>> val) { impl->set_v_str(name, val); }
void Yaml::set_v_real(cr<Str> name, cr<Vector<real>> val) { impl->set_v_real(name, val); }
Yaml Yaml::operator[] (cr<Str> name) const { return impl->operator[](name); }
Str Yaml::get_str(cr<Str> name, cr<Str> def) const { return impl->get_str(name, def); }
int Yaml::get_int(cr<Str> name, int def) const { return impl->get_int(name, def); }
real Yaml::get_real(cr<Str> name, real def) const { return impl->get_real(name, def); }
bool Yaml::get_bool(cr<Str> name, bool def) const { return impl->get_bool(name, def); }
utf8 Yaml::get_utf8(cr<Str> name, cr<utf8> def) const { return impl->get_utf8(name, def); }
Vector<Str> Yaml::get_v_str(cr<Str> name, cr<Vector<Str>> def) const { return impl->get_v_str(name, def); }
Vector<int> Yaml::get_v_int(cr<Str> name, cr<Vector<int>> def) const { return impl->get_v_int(name, def); }
Vector<real> Yaml::get_v_real(cr<Str> name, cr<Vector<real>> def) const { return impl->get_v_real(name, def); }
Yaml::vkv_t Yaml::get_kv_table() const { return impl->get_kv_table(); }
Yaml::vkvu32_t Yaml::get_kvu32_table() const { return impl->get_kvu32_table(); }
Strs Yaml::root_tags() const { return impl->root_tags(); }
bool Yaml::check() const { return impl->check(); }
Vector<Yaml> Yaml::items() const { return impl->items(); }
