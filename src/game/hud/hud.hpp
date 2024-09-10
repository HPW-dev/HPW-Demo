#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/unicode.hpp"

class Image;
struct Vec;

// База для интерфейса игрока
class Hud {
  nocopy(Hud);

public:
  Hud() = default;
  virtual ~Hud() = default;
  virtual void draw(Image& dst) const = 0;
  virtual void update(const Delta_time dt) = 0;
  // рисует прозрачный текст с чёрными контурами
  static void draw_expanded_text(Image& dst, cr<utf32> txt, const Vec pos);
};
