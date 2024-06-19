#pragma once
#include <deque>
#include <functional>
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/math/vec.hpp"

class Level;
class Image;

// Управление уровнями
class Level_mgr final {
public:
  using Maker = std::function< Shared<Level> () >;
  using Makers = std::deque<Maker>;

  bool end_of_levels {false}; // так можно проверить что игра кончилась
  // чтобы между уровнями игрок ставился в одно и то же место
  Vec m_player_pos_from_prev_level {};

  explicit Level_mgr(CN<Makers> _makers = {});
  ~Level_mgr() = default;
  // завершить текущий уровень
  void finalize_level();
  // @param vel смещение фона уровня (внутри dt на него не влияет)
  void update(const Vec vel, Delta_time dt);
  // отрисовка нижнего слоя уровня
  void draw(Image& dst) const;
  // отрисовка вехрнего слоя уровня
  void draw_upper_layer(Image& dst) const;
  // позиция игрока с предыдущего уровня
  Vec player_prev_lvl_pos() const;
  void set_player_prev_pos(const Vec pos);
  // узнать название уровня
  Str level_name() const;
  // управляет видимостью фона
  void set_visible(bool mode);
  // установить текущий уровень
  void set(CN<Maker> maker);

private:
  nocopy(Level_mgr);
  Makers m_makers {}; // конструкторы уровней
  Shared<Level> m_level {}; // текущий уровень
  bool m_visible {true}; // if false - disable redraw bg

  void accept_maker();
}; // Level_mgr
