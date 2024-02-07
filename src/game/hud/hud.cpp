#include "hud.hpp"
#include "hud-asci-1.hpp"
#include "hud-simple.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/game-common.hpp"
#include "game/game-hud.hpp"

void draw_hud(Image& dst) {
  return_if (!hpw::entity_mgr->get_player());

  switch (graphic::hud_mode) {
    default:
    case hud_e::simple: draw_hud_1(dst); break;
    case hud_e::asci_1: draw_hud_asci_1(dst); break;
  }
}
