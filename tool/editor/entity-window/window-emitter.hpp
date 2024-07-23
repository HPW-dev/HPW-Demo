#pragma once
#include "window.hpp"

// база для окон для спавнеров в entity-editor
class Window_emitter: public Window {
  bool m_active {true}; // false - выключит окно
public:
  bool active() const { return m_active; }
};
