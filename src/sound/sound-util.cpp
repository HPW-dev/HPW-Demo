#include "sound.hpp"
#include "sound-util.hpp"
#include "util/log.hpp"

namespace sound {

void kill_all() {
  Track::Config cfg { .playing = false };
  
  log_debug << "kill all tracks";
  auto tracks = all_tracks();
  for (crauto tr: tracks)
    tr->set_config(cfg);
}

void stop_all() {
  Track::Config cfg { .stoped = true };
  
  log_debug << "stop all tracks";
  auto tracks = all_tracks();
  for (crauto tr: tracks)
    tr->set_config(cfg);
}

void continue_all() {
  Track::Config cfg { .stoped = false };
  
  log_debug << "continue all tracks";
  auto tracks = all_tracks();
  for (crauto tr: tracks)
    tr->set_config(cfg);
}

} // sound ns
