#include "menu-test.hpp"
#include "game/util/game-util.hpp"
#include "game/scene/scene.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/font/unifont-mono.hpp"
#include "util/file/archive.hpp"

// вложенная тестовая сцена
class Nested_scene final: public Scene {
  Shared<Menu> menu {};

public:
  Nested_scene() {
    init_shared( menu, Menu_items{
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
    init_shared( menu, Menu_items{
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
  load_fonts();
  init_scene_mgr();
  hpw::scene_mgr->add(new_shared<Test_scene>());
}

Menu_test::~Menu_test() {}

void Menu_test::update(const Delta_time dt) { hpw::scene_mgr->update(dt); }
void Menu_test::draw() const { hpw::scene_mgr->draw(*graphic::canvas); }
