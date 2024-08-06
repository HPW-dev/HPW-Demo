#pragma once
#include "host/host.hpp"
#include "util/mem-types.hpp"

// рендерер в ASCI-графике
class Host_asci: public Host {
private:
  struct Impl;
  Unique<Impl> impl {};

public:
  explicit Host_asci(int argc, char** argv);
  ~Host_asci();
  void run() override;

protected:
  Delta_time get_time() const override;
  void draw_game_frame() const override;

  inline void _set_resize_mode(Resize_mode mode) override {}
  inline void _set_double_buffering(bool enable) override {}
  inline void _set_fullscreen(bool enable) override {}
  inline void _set_mouse_cursour_mode(bool mode) override {}
  inline void set_gamma(const double gamma) override {}
};
