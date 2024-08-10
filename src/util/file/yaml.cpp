#define YAML_CPP_API
#include <yaml-cpp/yaml.h>
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

// Внутреннаяя реализация для Yaml
class Yaml::Impl: public Resource {
  YAML::Node self {};

  inline void get_kv_table_helper(vkv_t &v_kv, CN<Str> key, CN<decltype(self)> v_node) const {
    if (v_node.size() == 0) {
      v_kv.push_back(kv_t{ .key=str_tolower(key), .value=v_node.as<Str>() });
    } else {
      for (auto node: v_node)
        get_kv_table_helper(v_kv, key + "." + node.first.as<Str>(), node.second);
    }
  }

  inline void get_kv32_table_helper(vkvu32_t &v_kv32, CN<Str> key, CN<decltype(self)> v_node) const {
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
  inline void set_v(CN<Str> name, CN<Vector<T>> val) {
    cfor (i, val.size())
      self[name][i] = val[i];
  }

  // helper for get value from YAML
  template <class T>
  inline T _get(CN<Str> name, CN<T> default_val) const {
    try {
      return self[name].as<T>();
    } catch (...) {
      detailed_log("\nWARNING: Yaml._get: not finded \"" << name
        << "\" in file \"" << Resource::get_path()
        << "\". Loaded default:" << default_val << "\n");
      return default_val;
    }
  } // _get

  // версия для utf8 строк
  inline utf8 _get_utf8(CN<Str> name, CN<utf8> default_val) const {
    try {
      return sconv<utf8>(self[name].as<Str>());
    } catch (...) {
      detailed_log("\nWARNING: Yaml._get: not finded \"" << name
        << "\" in file \"" << Resource::get_path()
        << "\". Loaded default str\n");
      return default_val;
    }
  } // _get utf8

  template <class T>
  inline Vector<T> _get_v(CN<Str> name, CN<Vector<T>> default_val) const {
    try {
      Vector<T> ret;
      auto items = self[name];
      iferror(items.size() == 0, "нет элементов с таким тегом");
      for (cnauto str: items)
        ret.emplace_back(str.as<T>());
      return ret;
    } catch (...) {
      detailed_log("\nWARNING: Yaml._get_v: not finded \"" << name
        << "\" in file \"" << Resource::get_path()
        << "\". Loaded default value\n");
    }
    return default_val;
  } // _get_v

public:
  inline Impl(): Resource() {}
  
  inline Impl(CN<Impl> other) noexcept: self {other.self}
    { Resource::operator = (other); }

  inline Impl(Impl&& other) noexcept: self {std::move(other.self)}
    { Resource::operator = (std::move(other)); }
  inline ~Impl() = default;

  inline Impl(Str fname, bool make_if_not_exist) {
    conv_sep(fname);
    Resource::set_path(fname);
    detailed_log("Yaml: loading \"" << fname << "\"\n");

    try {
      self = YAML::LoadFile(fname);
    } catch (CN<YAML::BadFile> ex) { // если файла нет, то создать его с нуля
      if (make_if_not_exist) {
        hpw_log("файл \"" << fname << "\" отсутствует. Пересоздание\n")
        std::ofstream file(fname);
        file.close();
      }
      self = YAML::LoadFile(fname);
    }
  } // c-tor(fname, make_if_not_exist)

  inline Impl(CN<File> file)
  : Resource(file.get_path()) {
    detailed_log("Yaml: loading from memory \"" << file.get_path() << "\"\n");
    self = YAML::Load({file.data.begin(), file.data.end()});
  }

  inline Impl(CN<Yaml> other) { this->operator=(other); }
  inline Impl(Yaml&& other) { this->operator=(std::move(other)); }

  inline Impl(CN<YAML::Node> node, CN<Str> new_path)
  : Resource(new_path)
  , self(node)
  {}

  inline Impl& operator = (CN<Yaml> other) {
    if (std::addressof(self) != std::addressof(other.impl->self)) {
      self = other.impl->self;
      Resource::operator=(other);
    }
    return *this;
  }

  inline Impl& operator = (Yaml&& other) noexcept {
    if (std::addressof(self) != std::addressof(other.impl->self)) {
      self = std::move(other.impl->self);
      Resource::operator=(std::move(other));
    }
    return *this;
  }

  inline void save(Str fname) const {
    assert( !fname.empty());
    conv_sep(fname);
    detailed_log("save to file \"" << fname << "\"\n");
    std::ofstream file(fname, std::ios_base::trunc); // удалить старый файл
    YAML::Emitter emt;
    emt << self;
    file << emt.c_str();
  }

  inline void clear() { self = {}; }

  inline Yaml make_node(CN<Str> name) {
    self[name] = YAML::Node(YAML::NodeType::Null);
    return Impl(self[name], Resource::get_path());
  }

  inline Yaml make_node_if_not_exist(CN<Str> name) {
    if (auto node = Impl(self[name], Resource::get_path()); node.check())
      return node;
    return make_node(name);
  }

  inline void delete_node(CN<Str> name) { self.remove(name); }

  inline void set_int(CN<Str> name, int val) { self[name] = val; }
  inline void set_real(CN<Str> name, real val) { self[name] = val; }
  inline void set_bool(CN<Str> name, bool val) { self[name] = val; }
  inline void set_str(CN<Str> name, CN<Str> val) { self[name] = val; }
  inline void set_utf8(CN<Str> name, CN<utf8> val) { self[name] = sconv<Str>(val); }
  inline void set_v_int(CN<Str> name, CN<Vector<int>> val) { set_v(name, val); }
  inline void set_v_str(CN<Str> name, CN<Vector<Str>> val) { set_v(name, val); }
  inline void set_v_real(CN<Str> name, CN<Vector<real>> val) { set_v(name, val); }
  inline Yaml operator[] (CN<Str> name) const {
    try {
      auto node = self[name];
      iferror(!node, "!node");
      return Impl(node, Resource::get_path());
    }  catch(...) {
      detailed_log ("WARNING: node \"" << name << "\" not finded in yaml \""
        << Resource::get_path() << "\"\n");
    }
    return {};
  }
  inline Str get_str(CN<Str> name, CN<Str> def) const { return _get(name, def); }
  inline int get_int(CN<Str> name, int def) const { return _get(name, def); }
  inline real get_real(CN<Str> name, real def) const { return _get(name, def); }
  inline bool get_bool(CN<Str> name, bool def) const { return _get(name, def); }
  inline utf8 get_utf8(CN<Str> name, CN<utf8> def) const  { return _get_utf8(name, def); }
  inline Vector<Str> get_v_str(CN<Str> name, CN<Vector<Str>> def) const { return _get_v(name, def); }
  inline Vector<int> get_v_int(CN<Str> name, CN<Vector<int>> def) const { return _get_v(name, def); }
  inline Vector<real> get_v_real(CN<Str> name, CN<Vector<real>> def) const { return _get_v(name, def); }

  inline Yaml::vkv_t get_kv_table() const {
    vkv_t v_kv;
    for (auto node: self)
      get_kv_table_helper(v_kv, node.first.as<Str>(), node.second);
    return v_kv;
  }

  inline Yaml::vkvu32_t get_kvu32_table() const {
    vkvu32_t ret;
    for (auto node: self)
      get_kv32_table_helper(ret, node.first.as<Str>(), node.second);
    return ret;
  }

  inline Strs root_tags() const {
    Strs ret;
    for (auto node: self)
      ret.push_back(node.first.as<Str>());
    return ret;
  }

  inline bool check() const { return self && self.size(); }
}; // Impl

Yaml::Yaml(Str fname, bool make_if_not_exist)
: impl {new_unique<Impl>(fname, make_if_not_exist)}
{
  Resource::operator=(*impl);
}

Yaml::Yaml(CN<File> file)
: impl {new_unique<Impl>(file)}
{
  Resource::operator=(*impl);
}

Yaml::Yaml(Yaml&& other) noexcept
: impl {new_unique<Impl>(other)}
{
  Resource::operator=(std::move(*impl));
}

Yaml::Yaml()
: impl {new_unique<Impl>()}
{}

Yaml::Yaml(CN<Yaml> other)
: impl {new_unique<Impl>(other)}
{
  Resource::operator=(other);
}

Yaml::Yaml(CN<Impl> new_impl)
: impl {new_unique<Impl>(new_impl)}
{
  Resource::operator=(*impl);
}

// деструктор должен бытьв .cpp файле, а то не выйдет запихнуть реализаццию в unique_ptr
Yaml::~Yaml() {}

Yaml& Yaml::operator = (CN<Yaml> other) {
  if (this != std::addressof(other)) {
    impl->operator =(other);
    Resource::operator=(*impl);
  }
  return *this;
}

Yaml& Yaml::operator = (Yaml&& other) noexcept {
  if (this != std::addressof(other)) {
    impl->operator =(std::move(other));
    Resource::operator=(*impl);
  }
  return *this;
}

void Yaml::save(Str fname) const { impl->save(fname); }
void Yaml::clear() { impl->clear(); }
Yaml Yaml::make_node(CN<Str> name) { return impl->make_node(name); }
Yaml Yaml::make_node_if_not_exist(CN<Str> name) { return impl->make_node_if_not_exist(name); }
void Yaml::delete_node(CN<Str> name) { impl->delete_node(name); }
void Yaml::set_int(CN<Str> name, int val) { impl->set_int(name, val); }
void Yaml::set_real(CN<Str> name, real val) { impl->set_real(name, val); }
void Yaml::set_bool(CN<Str> name, bool val) { impl->set_bool(name, val); }
void Yaml::set_str(CN<Str> name, CN<Str> val) { impl->set_str(name, val); }
void Yaml::set_utf8(CN<Str> name, CN<utf8> val) { impl->set_utf8(name, val); }
void Yaml::set_v_int(CN<Str> name, CN<Vector<int>> val) { impl->set_v_int(name, val); }
void Yaml::set_v_str(CN<Str> name, CN<Vector<Str>> val) { impl->set_v_str(name, val); }
void Yaml::set_v_real(CN<Str> name, CN<Vector<real>> val) { impl->set_v_real(name, val); }
Yaml Yaml::operator[] (CN<Str> name) const { return impl->operator[](name); }
Str Yaml::get_str(CN<Str> name, CN<Str> def) const { return impl->get_str(name, def); }
int Yaml::get_int(CN<Str> name, int def) const { return impl->get_int(name, def); }
real Yaml::get_real(CN<Str> name, real def) const { return impl->get_real(name, def); }
bool Yaml::get_bool(CN<Str> name, bool def) const { return impl->get_bool(name, def); }
utf8 Yaml::get_utf8(CN<Str> name, CN<utf8> def) const { return impl->get_utf8(name, def); }
Vector<Str> Yaml::get_v_str(CN<Str> name, CN<Vector<Str>> def) const { return impl->get_v_str(name, def); }
Vector<int> Yaml::get_v_int(CN<Str> name, CN<Vector<int>> def) const { return impl->get_v_int(name, def); }
Vector<real> Yaml::get_v_real(CN<Str> name, CN<Vector<real>> def) const { return impl->get_v_real(name, def); }
Yaml::vkv_t Yaml::get_kv_table() const { return impl->get_kv_table(); }
Yaml::vkvu32_t Yaml::get_kvu32_table() const { return impl->get_kvu32_table(); }
Strs Yaml::root_tags() const { return impl->root_tags(); }
bool Yaml::check() const { return impl->check(); }
