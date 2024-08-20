#include "bonus-loader.hpp"
#include "util/math/vec.hpp"
#include "util/file/yaml.hpp"
#include "util/error.hpp"

struct Bonus_loader::Impl {
  inline explicit Impl(cr<Yaml> config) {
    // TODO
  }

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    error("нужна реализация");
  }
}; // Impl

Bonus_loader::Bonus_loader(cr<Yaml> config)
: impl {new_unique<Impl>(config)}
{}

Bonus_loader::~Bonus_loader() {}

Entity* Bonus_loader::operator()(Entity* master, const Vec pos, Entity* parent)
{ return impl->operator()(master, pos, parent); }
