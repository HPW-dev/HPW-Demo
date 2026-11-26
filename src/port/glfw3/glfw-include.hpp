#pragma once

#include <glad/gl.h>
// GLAD ниже GLFW не ставить. GL/glcorearb.h не подключать

#ifdef WINDOWS
  #define GLFW_DLL
#endif
#include <GLFW/glfw3.h>
