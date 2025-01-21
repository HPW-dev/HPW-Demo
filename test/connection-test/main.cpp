#include "game/core/canvas.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "host/glfw3/host-glfw.hpp"
#include "graphic/image/image.hpp"

class Test_app final: public Host_glfw {
public:
  inline explicit Test_app(int argc, char *argv[]): Host_glfw(argc, argv) {}

protected:
  void draw_game_frame() const override {
    rauto dst {*graphic::canvas};
    dst.fill(Pal8::red);
    Host_glfw::draw_game_frame();
  }

private:
  void init() override {
    Host_glfw::init();
    // TODO
  }

  void update(const Delta_time dt) override {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::soft_exit();
    
    // TODO
  }
}; // Test_app

int main(int argc, char *argv[]) {
  Test_app app(argc, argv);
  app.run();
}