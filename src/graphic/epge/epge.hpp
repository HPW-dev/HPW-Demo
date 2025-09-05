#pragma once
#include "epge-util.hpp"
#include "epge-params.hpp"
#include "util/math/num-types.hpp"
#include "util/unicode.hpp"
#include "graphic/image/image-fwd.hpp"

namespace epge {

// интерфейс для встроенных плагинов
class Base {
public:
  Base() = default;
  virtual ~Base() = default;
  virtual Str name() const noexcept = 0;
  virtual utf32 localized_name() const = 0;
  // коммент с описанием плагина
  virtual inline utf32 desc() const noexcept { return {}; }
  virtual void draw(Image& dst) const noexcept = 0;
  virtual inline void update(const Delta_time dt) noexcept {}
  // узнать какие параметры есть у плагина
  virtual Params params() noexcept = 0;
};

} // epge ns

// для автоматического добавление класса эффекта в список эффектов
template <class T> struct Epge_registrator { inline Epge_registrator() { add_epge<T>(); } };

#define EPGE_CLASS_MAKER(NAME) \
namespace epge { \
  class NAME: public epge::Base { \
    struct Impl; \
    Unique<Impl> impl {}; \
  public: \
    NAME(); \
    ~NAME(); \
    Str name() const noexcept override final; \
    utf32 desc() const noexcept override final; \
    utf32 localized_name() const override final; \
    void draw(Image& dst) const noexcept override final; \
    Params params() noexcept override final; \
  }; \
  inline Epge_registrator<NAME> _ignore_##NAME {}; \
}

#define EPGE_IMPL_MAKER(NAME) \
NAME::NAME(): impl{new_unique<Impl>()} {} \
NAME::~NAME() {} \
Str NAME::name() const noexcept { return impl->name(); } \
utf32 NAME::localized_name() const { return impl->localized_name(); } \
utf32 NAME::desc() const noexcept { return impl->desc(); } \
void NAME::draw(Image& dst) const noexcept { impl->draw(dst); } \
Params NAME::params() noexcept { return impl->params(); }
