#pragma once
extern "C" {
  #include <GL/glew.h>
  #ifdef WINDOWS
    #include <GL/wglew.h>
  #else
    #include <GL/glxew.h>
  #endif
  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glcorearb.h>
}
