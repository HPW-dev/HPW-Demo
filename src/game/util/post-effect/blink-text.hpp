#pragma once
#include "post-effects.hpp"
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "util/macro.hpp"

// рисует на экране мигающий текст в течении некоторого времени
class Blink_text: public Effect_mgr::Effect {
  struct Impl;
  Unique<Impl> impl {};

public:
  Blink_text() = default;
  ~Blink_text();
  explicit Blink_text(const Delta_time timeout, cr<utf32> txt);
  bool update(const Delta_time dt) override;
  void draw(Image& dst) const override;
};
