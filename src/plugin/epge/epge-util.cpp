#include <cassert>
#include <unordered_map>
#include <functional>
#include "epge-util.hpp"
#include "game/core/epges.hpp"
#include "util/file/yaml.hpp"
#include "scanline.hpp"

using Epge_maker = std::function< Unique<epge::Base> ()>;

namespace {
static std::unordered_map<Str, Epge_maker> _epge_makers {};
}

void save_epges() {
  // TODO
}

void load_epges() {
  // TODO
}

template <class T>
inline void add_epge() {
  cauto epge = new_unique<T>();
  cauto name = epge->name();
  ::_epge_makers[name] = []{ return new_unique<T>(); };
}

inline static void init_epge_list() {
  add_epge<epge::Scanline>();
};

Strs avaliable_epges() {
  if (::_epge_makers.empty())
    init_epge_list();

  Strs list;
  for (crauto [name, _]: ::_epge_makers)
    list.push_back(name);
  assert(!list.empty());
  return list;
}

Unique<epge::Base> make_epge(cr<Str> name) {
  if (::_epge_makers.empty())
    init_epge_list();

  return ::_epge_makers.at(name) (); // создать EPGE
}
