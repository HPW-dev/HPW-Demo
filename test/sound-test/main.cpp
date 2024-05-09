#include <iostream>
#include <thread>
#include "sound/sound-manager.hpp"
#include "sound/audio-io.hpp"
#include "util/error.hpp"

void test_sine() {
  std::cout << "\nAudio test: playing sine wave" << std::endl;


} // test_sine

void test_file() {
  std::cout << "\nAudio test: loading file" << std::endl;

  // загрузить звук с диска
  cauto track = load_audio("test.opus");
  Sound_mgr sound_mgr;
  sound_mgr.add_audio("test tack 1", track);
  
  // проиграть звук
  cauto audio_id = sound_mgr.play("test tack 1");
  iferror(audio_id == BAD_AUDIO, "Bad audio ID");
  std::cout << "track \"" << track.get_path() << "\" is ";
  std::cout << (sound_mgr.is_playing(audio_id) ? "playing" : "stoped") << std::endl;

  // не закрывать прогу, пока трек играет
  while (sound_mgr.is_playing(audio_id)) {
    std::this_thread::yield();
  }
} // test_file

int main() {
  test_sine();
  test_file();
}
