#pragma once
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "host.hpp"
#include "resize.hpp"

// окно без рендерера
class Protownd: public Host {
  nocopy(Protownd);

protected:
  Vector<float> pal_rgb {};
  int w_ = 0, h_ = 0; // разрешение экрана
  Resize_ctx window_ctx_ {};

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
