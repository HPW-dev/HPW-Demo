#include "animation-helper.hpp"
#include "game/core/anims.hpp"
#include "graphic/animation/anim-io.hpp"

void load_animations() {
  init_unique(hpw::anim_mgr);

  if (!hpw::lazy_load_anim) {
    cauto anim_yml = get_anim_config();
    read_anims(anim_yml);
  }
}
