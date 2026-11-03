#pragma once
extern "C" {
  #include <glad/gl.h>

  #ifndef WINDOWS
  #include <GL/glxew.h>
  #endif

  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glcorearb.h>
}
