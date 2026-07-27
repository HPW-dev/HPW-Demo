#pragma once
#include <atomic>
#include "game/util/keybits.hpp"

extern "C" {
  #include "host/ogl3/ogl.hpp"
  #ifdef WINDOWS
    #define GLFW_DLL
  #else
    #include <unistd.h>
  #endif
  #include <GLFW/glfw3.h>
}

struct Host_glfw;

inline std::atomic<Host_glfw*> g_instance {};
inline bool g_rebind_key_mode {false};
// позволяет избежать зацикливания при выставлении стандартной гаммы при ошибке
inline bool g_set_default_gamma_once {true};
// появится при hpw::rebind_key
inline hpw::keycode g_key_for_rebind {hpw::keycode::error};
