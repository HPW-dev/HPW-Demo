#include "menu-test.hpp"
#include "game/util/game-util.hpp"
#include "game/scene/scene.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/unifont.hpp"
#include "util/file/archive.hpp"

// вложенная тестовая сцена
class Nested_scene final: public Scene {
  Shared<Menu> menu {};

public:
  Nested_scene() {
    menu = new_shared<Text_menu>( Menu_items{
      new_shared<Menu_text_item>(U"none 2"),
      new_shared<Menu_text_item>(U"back", [](){ hpw::scene_mgr->back(); }),
    },
    Vec{50, 50} );
  }
  ~Nested_scene() = default;
  void update(const Delta_time dt) override { menu->update(dt); }
  void draw(Image& dst) const override { menu->draw(dst); }
}; // Nested_scene

// начальная тестовая сцена
class Test_scene final: public Scene {
  Shared<Menu> menu {};

public:
  Test_scene() {
    menu = new_shared<Text_menu>( Menu_items{
      new_shared<Menu_text_item>(U"next", []{
        hpw::scene_mgr->add(new_shared<Nested_scene>()); }),
      new_shared<Menu_text_item>(U"none"),
      new_shared<Menu_text_item>(U"back", []{ hpw::scene_mgr->back(); }),
    },
    Vec{30, 30} );
  }
  ~Test_scene() = default;
  void update(const Delta_time dt) override { menu->update(dt); }
  void draw(Image& dst) const override { menu->draw(dst); }
}; // Test_scene

void Menu_test::init(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  init_archive();
  auto font_file {hpw::archive->get_file(
    "resource/font/unifont-13.0.06.ttf")};
  graphic::font = new_shared<Unifont>(font_file, 16));
  init_scene_mgr();
  hpw::scene_mgr->add(new_shared<Test_scene>());
}

Menu_test::~Menu_test() {}

void Menu_test::update(const Delta_time dt) { hpw::scene_mgr->update(dt); }
void Menu_test::draw() const { hpw::scene_mgr->draw(*graphic::canvas); }
