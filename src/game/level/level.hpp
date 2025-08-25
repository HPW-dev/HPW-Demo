#pragma once
#include <functional>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

// База для уровней
class Level {
public:
  // if true - level complete. Use Level_mgr::finalize_level
  bool m_complete {false};
  // какое действие выполнить при смерти игрока
  std::function<void ()> on_player_death_action {};

  Level();
  virtual ~Level();
  // @param vel - смещение фона уровня (внутри dt на него не влияет)
  virtual void update(const Vec vel, Delta_time dt);
  // отрисовка на уровне нижнего фона
  virtual void draw(Image& dst) const = 0;
  // отрисовка на уровне выше (post draw)
  inline virtual void draw_upper_layer(Image& dst) const {}
  virtual Str level_name() const = 0;

private:
  nocopy(Level);
  struct Impl;
  Unique<Impl> _impl {};
};
