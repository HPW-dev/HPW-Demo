#pragma once
#include <optional>
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"
#include "util/math/vec.hpp"

/** игрок управляется именно с виртуального джойстика
* @details Угол поворота определяется стиком или зажатыми кнопками на крестовине */
class Virtual_joystick {
  struct Impl;
  Unique<Impl> _impl{};

public:
  // доступные кнопки
  enum class Key {IGNORE, A, B, MENU, ENABLE};

  void press(Key key); // зажать кнопку
  void release(Key key); // отпустить кнопку
  bool pressed(Key key) const; // кнопка зажата
  bool released(Key key) const; // кнопку отпустили
  void set_angle(real degree, real intense);
  real get_degree() const; // в какой угол движемся
  real get_intense() const; // с какой силой движемся
  Vec direction() const; // вектор движения (длина от 0 до 1)
  Virtual_joystick();
  ~Virtual_joystick();
};

namespace hpw {
  inline Virtual_joystick virtual_joystick; // основной контроллер игрока
}
