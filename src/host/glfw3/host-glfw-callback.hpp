#pragma once
#include <cstdint>
#include "util/str.hpp"

struct GLFWwindow;

void key_callback(GLFWwindow* m_window, int key, int scancode, int action, int mods);
void error_callback(int error, Cstr description);
void reshape_callback(GLFWwindow* m_window, int w, int h);
void utf32_text_input_cb(GLFWwindow* m_window, std::uint32_t codepoint) ;
