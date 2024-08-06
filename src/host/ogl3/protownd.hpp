#pragma once
#include <GL/glew.h>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "host/host.hpp"
#include "host/host-resize.hpp"

// окно без рендерера
class Protownd: public Host {
  nocopy(Protownd);

protected:
  Vector<GLfloat> m_pal_rgb {};
  int m_w = 0, m_h = 0; // разрешение экрана
  Resize_ctx m_window_ctx {};

  void save_screenshot() const override;
  void _gen_palette();
  virtual void ogl_resize(int w, int h) = 0;
  void _set_resize_mode(Resize_mode mode) override;

public:
  explicit Protownd(int argc, char *argv[]);
  ~Protownd() = default;
  virtual void reshape(int w, int h);
  virtual void set_window_pos(int x, int y);
}; // Protownd
