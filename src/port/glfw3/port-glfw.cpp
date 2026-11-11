#include "pch.hpp"
#include "port-glfw.hpp"

#include <csignal>

#ifdef WINDOWS
  #define GLFW_DLL
#else
  #include <unistd.h>
#endif

#include <glad/gl.h>
// GLAD ниже GLFW не ставить. GL/glcorearb.h не подключать
#include <GLFW/glfw3.h>


struct Glfw3_context {
  GLFWwindow* window {};
  bool is_ran = true;
};
// Все нужные данные для GLFW окна
static Glfw3_context g_ctx {};


void signal_handler(int signal) {
  if (signal == SIGINT) {
    log_info << "Закрытие через SIGINT...";
    g_ctx.is_ran = false;
  }
}

//---- Основные функции GLFW3 порта --------------------------------------------

void glfw3_init() {
  log_info << "GLFW3 initialization...";

  std::signal(SIGINT, signal_handler); // для закрытия через Ctrl+C 
  // TODO

  log_debug << "GLFW3 end of initialization";
}

bool glfw3_is_ran() {
  return g_ctx.is_ran && (glfwWindowShouldClose(g_ctx.window) == GL_FALSE);
}

void glfw3_update() {
  // TODO
}

void glfw3_render_canvas() {
  // TODO
}

void glfw3_sync() {
  // TODO
}

void glfw3_finalize() {
  log_info << "GLFW3 finalization...";
  // TODO
  log_debug << "GLFW3 end of finalization";
}
