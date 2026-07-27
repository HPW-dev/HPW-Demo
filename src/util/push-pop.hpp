#pragma once

// Сейвит состояние, при деструкторе возвращает  сохранённое состояние
template <class T>
class push_pop {
  T& _ref;
  T _value {};

public:
  inline explicit push_pop(T& src): _ref {src}, _value {src} {}
  inline virtual ~push_pop() { _ref = _value; }
};
