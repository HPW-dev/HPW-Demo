#include "pch.hpp"
#include "animation-helper.hpp"
#include "game/core/anims.hpp"
#include "engine/graphic/animation/anim-io.hpp"

void load_animations() {
  init_unique(hpw::anim_mgr);

  if (!hpw::lazy_load_anim)
    read_anims( get_anim_config() );
}
