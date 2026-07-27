#pragma once
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "scene.hpp"

// окно смены языка
class Scene_locale_select final: public Scene {
  struct Impl;
  Unique<Impl> impl {};

public:
  constx Str NAME = "locale select";
  Scene_locale_select();
  ~Scene_locale_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }
};

namespace hpw {
inline utf32 locale_select_title = U"Язык • Language • 言語 • 언어";
}
