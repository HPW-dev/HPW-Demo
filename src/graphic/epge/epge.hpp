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
  virtual Str name() const = 0;
  virtual utf32 localized_name() const = 0;
  // коммент с описанием плагина
  virtual inline utf32 desc() const { return {}; }
  virtual void draw(Image& dst) const = 0;
  virtual inline void update(const Delta_time dt) {}
  // узнать какие параметры есть у плагина
  virtual Params params() = 0;
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
    Str name() const; \
    utf32 desc() const; \
    utf32 localized_name() const; \
    void draw(Image& dst) const; \
    void update(const Delta_time dt); \
    Params params(); \
  }; \
  inline Epge_registrator<NAME> _ignore_##NAME {}; \
}

// старый способ инициализации реализации эффекта
#define EPGE_IMPL_MAKER(NAME) \
NAME::NAME(): impl{new_unique<Impl>()} {} \
NAME::~NAME() {} \
Str NAME::name() const { return impl->name(); } \
utf32 NAME::localized_name() const { return impl->localized_name(); } \
utf32 NAME::desc() const { return impl->desc(); } \
void NAME::draw(Image& dst) const { impl->draw(dst); } \
void NAME::update(const Delta_time dt) { impl->update(dt); } \
Params NAME::params() { return impl->params(); }

// новый способ инициализации эффекта
#define EPGE_CLASS_BEGIN(EFFECT_NAME) \
namespace epge { \
class CONCAT(Epge_, EFFECT_NAME): public epge::Base { \
public: \
  inline Str name() const { return STRINGIFY(EFFECT_NAME); } \
  inline utf32 localized_name() const { return get_locale_str("epge.effect." STRINGIFY(EFFECT_NAME) ".name"); } \
  inline utf32 desc() const { return get_locale_str("epge.effect." STRINGIFY(EFFECT_NAME) ".desc"); } \
private:

#define EPGE_CLASS_END(EFFECT_NAME) \
}; \
inline Epge_registrator<CONCAT(Epge_, EFFECT_NAME)> CONCAT(_registrator_for_, EFFECT_NAME); \
}
