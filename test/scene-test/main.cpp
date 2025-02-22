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

Str get_scenes_list() {
  cauto list = hpw::scene_mgr.list();
  Str result {};

  for (crauto name: list)
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

  const Str question = "A B C D ";
  cauto result = get_scenes_list();
  iferror(result != question, "test 1 failed");
}

int main() {
  try {
    test_1();
  } catch(cr<hpw::Error> err) {
    hpw::Logger::config.print_source = false;
    log_error << err.what();
    hpw::Logger::config.print_source = true;
    return EXIT_FAILURE;
  }
  
  log_info << "=== All scene tests complited";
}
