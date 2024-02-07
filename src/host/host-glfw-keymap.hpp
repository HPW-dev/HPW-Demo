#pragma once
#include <unordered_map>
#include "util/macro.hpp"
#include "util/unicode.hpp"

struct GLFWwindow;
struct Keys_info;

namespace hpw { enum class keycode; }

//! перенаправляет управление с GLFW в игру
class Key_mapper final {
  nocopy(Key_mapper);

  struct GLFW_key {
    utf32 name {};
    int scancode {}; /// системный код клавиши
  };

  /// <hpw key, GLFW key>
  std::unordered_map<hpw::keycode, GLFW_key> table {};

public:
  Key_mapper();
  ~Key_mapper() = default;
  void reset(); /// поставить клавиши по умолчанию
  void bind(hpw::keycode dst_hpw_key, int glfw_key); /// забиндить клавишу
  inline decltype(table)& get_table() { return table; }
  Keys_info get_info() const;
};
