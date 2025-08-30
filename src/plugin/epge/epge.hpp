#pragma once
#include "epge-util.hpp"
#include "epge-params.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/image-fwd.hpp"

namespace epge {

// интерфейс для встроенных плагинов
class Base {
public:
  Base() = default;
  virtual ~Base() = default;
  virtual Str name() const noexcept = 0;
  virtual inline Str desc() const noexcept { return {}; } // коммент с описанием плагина
  virtual void draw(Image& dst) const noexcept = 0;
  virtual inline void update(const Delta_time dt) noexcept {}
  // узнать какие параметры есть у плагина
  virtual Params params() noexcept = 0;
};

} // epge ns

template <class T> struct Epge_registrator { inline Epge_registrator() { add_epge<T>(); } };

#define EPGE_CLASS_MAKER(NAME) \
namespace epge { \
  class NAME: public epge::Base { \
    struct Impl; \
    Unique<Impl> impl {}; \
  public: \
    NAME(); \
    ~NAME(); \
    Str name() const noexcept final; \
    Str desc() const noexcept final; \
    void draw(Image& dst) const noexcept final; \
    Params params() noexcept final; \
  }; \
  inline Epge_registrator<NAME> _ignore_##NAME {}; \
}

#define EPGE_IMPL_MAKER(NAME) \
NAME::NAME(): impl{new_unique<Impl>()} {} \
NAME::~NAME() {} \
Str NAME::name() const noexcept { return impl->name(); } \
Str NAME::desc() const noexcept { return impl->desc(); } \
void NAME::draw(Image& dst) const noexcept { impl->draw(dst); } \
Params NAME::params() noexcept { return impl->params(); }
