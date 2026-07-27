#pragma once
#include "direct.hpp"
#include "graphic/util/rotsprite.hpp"
#include "util/vector-types.hpp"
#include "util/str.hpp"

// кадр анимации с разными вариантами поворота
class Frame final {
  Vector<Direct> directions {}; // повёрнутые версии кадра
  real degree_frag {}; // шаг угловой анимации
  Str name {}; // (для редактора)

  static Str generate_frame_name();

  void init_once(); // анимация с одним разворотом
  void init_quarters();
  void accept_degree_offset(real &degree) const;

public:
  // для конфигурирования инита поворотов
  struct Source_ctx {
    // исходная инфа о кадре (0 град.), можно использовать для реинита
    Direct direct_0 {};
    // для коррекции артефактов спрайтов при автоповороте
    Vec rotate_offset {};
    Color_get_pattern cgp {};
    Color_compute ccf {};
    std::size_t max_directions {};
  };

  Source_ctx source_ctx {};
  real duration {}; // длительность кадра
  real degree_offset {}; // смещение начального угла
  
  void init_directions(Source_ctx&& new_ctx);
  void reinit_directions_by_source();
  void clear_directions();
  // получить угол поворота по градусам поворота
  cp<Direct> get_direct(real degree) const;
  inline decltype(directions)& get_directions() { return directions; }
  inline cr<decltype(directions)> get_directions() const { return directions; }
  // получить смещения всех внутренних кадров поворота
  Vector<Vec> get_offsets() const;
  cr<Str> get_name() const;

  Frame();
  ~Frame() = default;
  Frame(cr<Frame> other);
  Frame& operator = (cr<Frame> other);
  Frame(Frame&& other) noexcept;
  Frame& operator = (Frame&& other) noexcept;
}; // Frame
