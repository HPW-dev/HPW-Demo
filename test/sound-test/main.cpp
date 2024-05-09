#include <iostream>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <thread>
#include <cmath>
#include "sound/sound-manager.hpp"
#include "sound/audio-io.hpp"
#include "util/error.hpp"
#include "util/vector-types.hpp"

void test_sine() {
  std::cout << "\nAudio test: playing sine wave" << std::endl;

  // сделать синусоиду
  Audio sine_wave;
  sine_wave.channels = 1;
  sine_wave.compression = Audio::Compression::raw;
  sine_wave.format = Audio::Format::pcm_f32;
  sine_wave.set_generated(true);
  sine_wave.frequency = 48'000;
  sine_wave.samples = sine_wave.frequency * 3;
  Vector<float> f32_wave(sine_wave.samples);
  constexpr float step_speed = 0.007; // влияет на частоту синусоиды
  float step {};
  for (nauto sample: f32_wave) {
    sample = std::sin(step);
    step += step_speed;
  }
  sine_wave.data.resize(f32_wave.size() * sizeof(float));
  std::memcpy(sine_wave.data.data(), f32_wave.data(), sine_wave.data.size());
  
  // добавить звук в базу
  Sound_mgr sound_mgr;
  sound_mgr.move_audio("sine wave", std::move(sine_wave));

  // проиграть звук
  cauto audio_id = sound_mgr.play("sine wave");

  // не закрывать прогу, пока трек играет
  while (true) {
    sound_mgr.update();
    break_if (!sound_mgr.is_playing(audio_id));
    std::this_thread::yield();
  }
} // test_sine

void test_motion_sine() {
  std::cout << "\nAudio test: motion sine wave" << std::endl;

  // сделать синусоиду
  Audio sine_wave;
  sine_wave.channels = 1;
  sine_wave.compression = Audio::Compression::raw;
  sine_wave.format = Audio::Format::pcm_f32;
  sine_wave.set_generated(true);
  sine_wave.frequency = 48'000;
  sine_wave.samples = sine_wave.frequency * 8;
  Vector<float> f32_wave(sine_wave.samples);
  constexpr float step_speed = 0.06; // влияет на частоту синусоиды
  float step {};
  for (nauto sample: f32_wave) {
    sample = std::sin(step);
    step += step_speed;
  }
  sine_wave.data.resize(f32_wave.size() * sizeof(float));
  std::memcpy(sine_wave.data.data(), f32_wave.data(), sine_wave.data.size());
  
  // добавить звук в базу
  Sound_mgr sound_mgr;
  sound_mgr.move_audio("sine wave", std::move(sine_wave));
  sound_mgr.set_listener_pos(Vec3(0, 5, 0));
  //sound_mgr.set_listener_dir(Vec3(0, -1, 0));

  // проиграть звук
  Vec3 pos(-15, 0, 0);
  Vec3 vel(1, 0, 0);
  cauto audio_id = sound_mgr.play("sine wave", pos, vel, 0.3);

  // не закрывать прогу, пока трек играет
  constexpr float speed = 0.00002;
  while (true) {
    pos.x += vel.x * speed;
    sound_mgr.set_velocity(audio_id, vel);
    sound_mgr.set_position(audio_id, pos);
    sound_mgr.update();
    break_if (!sound_mgr.is_playing(audio_id));
    std::this_thread::yield();
  }
} // test_motion_sine

void test_noise() {
  std::cout << "\nAudio test: playing noise" << std::endl;

  // сделать стерео шум
  Audio noise;
  noise.channels = 2;
  noise.compression = Audio::Compression::raw;
  noise.format = Audio::Format::raw_pcm_s16;
  noise.set_generated(true);
  noise.frequency = 22'100;
  noise.samples = noise.frequency * 3;
  noise.data.resize(noise.samples * sizeof(std::int16_t));
  for (nauto sample: noise.data)
    sample = rand() % 256;
  
  // добавить звук в базу
  Sound_mgr sound_mgr;
  sound_mgr.move_audio("noise", std::move(noise));

  // проиграть звук
  cauto audio_id = sound_mgr.play("noise", {}, {}, 0.01);

  // не закрывать прогу, пока трек играет
  while (true) {
    sound_mgr.update();
    break_if (!sound_mgr.is_playing(audio_id));
    std::this_thread::yield();
  }
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
  //test_sine();
  //test_noise();
  test_motion_sine();
  //test_file();
}
