#pragma once
#include "util/macro.hpp"
#include "util/num-types.hpp"
#include "util/vec.hpp"
#include "util/unicode.hpp"
#include "engine/graphic/image/image-fwd.hpp"

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
