#include "game/core/canvas.hpp"
#include "host/glfw3/host-glfw.hpp"
#include "game/util/locale.hpp"
#include "game/util/font-helper.hpp"
#include "game/core/scenes.hpp"
#include "game/core/locales.hpp"
#include "server-or-client.hpp"

class Test_app final: public Host_glfw {
public:
  inline explicit Test_app(int argc, char *argv[]): Host_glfw(argc, argv) {}

protected:
  void draw_game_frame() const override {
    hpw::scene_mgr.draw(*graphic::canvas);
    Host_glfw::draw_game_frame();
  }

private:
  void init() override {
    Host_glfw::init();
    load_locale(hpw::locale_path);
    load_fonts();
    hpw::scene_mgr.add(new_shared<Server_or_client>());
  }

  void update(const Delta_time dt) override {
    if (!hpw::scene_mgr.update(dt))
      hpw::soft_exit();
  }
}; // Test_app

int main(int argc, char *argv[]) {
  Test_app app(argc, argv);
  app.run();
}
