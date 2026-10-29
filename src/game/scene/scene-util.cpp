#include "pch.hpp"
#include "scene-util.hpp"

#include "game/scene/options/scene-palette-select.hpp"
#include "game/scene/options/scene-hud-select.hpp"
#include "game/scene/options/scene-bgp-select.hpp"
#include "game/scene/options/scene-graphic.hpp"
#include "game/scene/options/scene-epge.hpp"
#include "game/scene/gameplay/scene-game-pause.hpp"
#include "game/scene/gameplay/scene-gameover.hpp"
#include "game/scene/gameplay/scene-game.hpp"
#include "game/scene/common/scene-dummy.hpp"
#include "game/scene/common/scene-debug.hpp"
#include "game/scene/common/scene-cmd.hpp"
#include "scene-main-menu.hpp"


using Maker = std::function<Shared<Scene> ()>;

static std::unordered_map<Str, Maker> table {
#define ITEM(SCENE) {SCENE::NAME, []{ return new_shared<SCENE>(); } },
  ITEM(Scene_palette_select)
  ITEM(Scene_bgp_select)
  ITEM(Scene_game_pause)
  ITEM(Scene_hud_select)
  ITEM(Scene_main_menu)
  ITEM(Scene_gameover)
  ITEM(Scene_graphic)
  ITEM(Scene_dummy)
  ITEM(Scene_debug)
  ITEM(Scene_game)
  ITEM(Scene_epge)
  ITEM(Scene_cmd)
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
