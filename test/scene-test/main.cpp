#include "game/core/scenes.hpp"
#include "game/scene/scene.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

class Scene_empty: public Scene {
public:
  inline explicit Scene_empty(cr<Str> name): _name {name} {}
  inline cr<Str> name() const override { return _name; }

private:
  Str _name {"Scene_empty"};
};

Str get_scene_names() {
  cauto names = hpw::scene_mgr.names();
  Str result {};

  for (crauto name: names)
    result += name + " ";

  if (result.empty())
    result = "empty";

  log_info << "scenes: " << result;
  return result;
}

void test_1() {
  log_info << "=== Test 1 : Simple scene order";

  hpw::scene_mgr.add(new_shared<Scene_empty>("A"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("B"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("C"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("D"));
  cauto ret = hpw::scene_mgr.update(0.016);

  const Str question = "A B C D ";
  cauto result = get_scene_names();
  iferror(result != question, "bad result");
  iferror(hpw::scene_mgr.status().empty, "empty scenes");
  iferror(!hpw::scene_mgr.status().next_scene, "!next_scene (need next_scene == true)");
  iferror(hpw::scene_mgr.status().came_back, "came_back (need came_back == false)");
  iferror(!ret, "!ret (need ret == true)");
  hpw::scene_mgr.clear();
}

void test_2() {
  log_info << "=== Test 2 : Scene order with calling .back(2)";

  hpw::scene_mgr.add(new_shared<Scene_empty>("A"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("B"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("C"));
  hpw::scene_mgr.back(2);
  hpw::scene_mgr.add(new_shared<Scene_empty>("D"));
  cauto ret = hpw::scene_mgr.update(0.016);

  const Str question = "A D ";
  cauto result = get_scene_names();
  iferror(result != question, "bad result");
  iferror(hpw::scene_mgr.status().empty, "empty scenes");
  iferror(!hpw::scene_mgr.status().next_scene, "!next_scene (need next_scene == true)");
  iferror(!hpw::scene_mgr.status().came_back, "!came_back (need came_back == true)");
  iferror(!ret, "!ret (need ret == true)");
  hpw::scene_mgr.clear();
}

void test_3() {
  log_info << "=== Test 3 : Delete all scenes by .back(3)";

  hpw::scene_mgr.add(new_shared<Scene_empty>("A"));
  hpw::scene_mgr.add(new_shared<Scene_empty>("B"));
  hpw::scene_mgr.back(3);
  cauto ret = hpw::scene_mgr.update(0.016);

  const Str question = "empty";
  cauto result = get_scene_names();
  iferror(result != question, "bad result");
  iferror(!hpw::scene_mgr.status().empty, "!empty scenes (need for empty)");
  iferror(!hpw::scene_mgr.status().next_scene, "!next_scene (need next_scene == true)");
  iferror(!hpw::scene_mgr.status().came_back, "!came_back (need came_back == true)");
  iferror(ret, "ret (need ret == false)");
  hpw::scene_mgr.clear();
}

int main() {
  try {
    test_1();
    test_2();
    test_3();
  } catch(cr<hpw::Error> err) {
    hpw::Logger::config.print_source = false;
    log_error << err.what();
    hpw::Logger::config.print_source = true;
    return EXIT_FAILURE;
  }
  
  log_info << "=== All scene tests complited";
}
