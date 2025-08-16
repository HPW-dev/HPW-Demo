#include <functional>
#include <unordered_map>
#include "scene-util.hpp"
#include "scene-game.hpp"
#include "scene-epge.hpp"
#include "scene-debug.hpp"
#include "scene-cmd.hpp"
#include "scene-game-pause.hpp"
#include "scene-bgp-select.hpp"
#include "scene-gameover.hpp"
#include "scene-graphic.hpp"
#include "scene-hud-select.hpp"
#include "scene-main-menu.hpp"

using Maker = std::function<Shared<Scene> ()>;
static std::unordered_map<Str, Maker> table {
  #define ITEM(SCENE) {SCENE::NAME, []{ return new_shared<SCENE>(); } },
  ITEM(Scene_game)
  ITEM(Scene_epge)
  ITEM(Scene_debug)
  ITEM(Scene_cmd)
  ITEM(Scene_bgp_select)
  ITEM(Scene_game_pause)
  ITEM(Scene_gameover)
  ITEM(Scene_graphic)
  ITEM(Scene_hud_select)
  ITEM(Scene_main_menu)
  #undef ITEM
};

Strs scene_names() {
  Strs names;
  for (crauto [name, _]: table)
    names.push_back(name);
  return names;
}

Shared<Scene> find_scene(cr<Str> name) {
  return table.at(name)();
}
