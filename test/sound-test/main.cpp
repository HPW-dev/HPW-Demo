#include <iostream>
#include "sound/sound-manager.hpp"
#include "sound/audio-io.hpp"

int main() {
  Sound_mgr sound_mgr;
  cauto track = load_audio("test.opus");
  cauto context = sound_mgr.play(track);

  std::cout << "track \"" << track.get_path() << "\" is ";
  std::cout << (sound_mgr.is_playing(context) ? "playing" : "stoped") << std::endl;
}
