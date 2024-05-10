#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <thread>
#include <cmath>
#include "sound/sound-manager.hpp"
#include "sound/audio-io.hpp"
#include "util/error.hpp"
#include "util/vector-types.hpp"

Audio make_sin_wave(const float freq) {
  Audio sine_wave;
  sine_wave.channels = 1;
  sine_wave.compression = Audio::Compression::raw;
  sine_wave.format = Audio::Format::pcm_f32;
  sine_wave.set_generated(true);
  sine_wave.frequency = 48'000;
  sine_wave.samples = sine_wave.frequency * 4;
  Vector<float> f32_wave(sine_wave.samples * sine_wave.channels);
  float step {};
  for (nauto sample: f32_wave) {
    sample = std::sin(step);
    step += freq;
  }
  sine_wave.data.resize(sine_wave.samples * sizeof(float) * sine_wave.channels);
  std::memcpy(sine_wave.data.data(), f32_wave.data(), sine_wave.data.size());
  return sine_wave;
}

void test_sine() {
  std::cout << "\nAudio test: playing sine wave" << std::endl;

  // добавить звук в базу
  Sound_mgr sound_mgr;
  auto sine_wave = make_sin_wave(0.007);
  sound_mgr.move_audio("sine wave", std::move(sine_wave));

  // проиграть звук
  cauto audio_id = sound_mgr.play("sine wave");

  // не закрывать прогу, пока трек играет
  while (true) {
    break_if (!sound_mgr.is_playing(audio_id));
    std::this_thread::yield();
  }
} // test_sine

void test_motion_sine() {
  std::cout << "\nAudio test: motion sine wave" << std::endl;

  // добавить звук в базу
  Sound_mgr sound_mgr;
  auto sine_wave = make_sin_wave(0.02);
  sound_mgr.move_audio("sine wave", std::move(sine_wave));
  sound_mgr.set_listener_pos(Vec3(0, 0, 1));
  sound_mgr.set_doppler_factor(15);

  // проиграть звук
  Vec3 pos(-15, 0, 0);
  Vec3 vel(1, 0, 0);
  cauto audio_id = sound_mgr.play("sine wave", pos, vel, 0.3);

  // не закрывать прогу, пока трек играет
  constexpr float speed = 0.0000666;
  while (true) {
    pos.x += vel.x * speed;
    sound_mgr.set_velocity(audio_id, vel);
    sound_mgr.set_position(audio_id, pos);
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
  noise.data.resize(noise.samples * sizeof(std::int16_t) * noise.channels);
  for (nauto sample: noise.data)
    sample = rand() % 256;
  
  // добавить звук в базу
  Sound_mgr sound_mgr;
  sound_mgr.move_audio("noise", std::move(noise));

  // проиграть звук
  cauto audio_id = sound_mgr.play("noise", {}, {}, 0.01);

  // не закрывать прогу, пока трек играет
  while (true) {
    break_if (!sound_mgr.is_playing(audio_id));
    std::this_thread::yield();
  }
} // test_sine

void test_mix() {
  std::cout << "\nAudio test: mixing audio" << std::endl;
  auto track_1 = make_sin_wave(0.007);
  auto track_2 = make_sin_wave(0.01);
  auto track_3 = make_sin_wave(0.05);
  auto track_4 = make_sin_wave(0.1);
  Sound_mgr sound_mgr;
  sound_mgr.move_audio("sin 1", std::move(track_1));
  sound_mgr.move_audio("sin 2", std::move(track_2));
  sound_mgr.move_audio("sin 3", std::move(track_3));
  sound_mgr.move_audio("sin 4", std::move(track_4));
  using namespace std::chrono_literals;
  sound_mgr.play("sin 1");
  std::this_thread::sleep_for(1s);
  sound_mgr.play("sin 2");
  std::this_thread::sleep_for(1s);
  sound_mgr.play("sin 3");
  std::this_thread::sleep_for(1s);
  sound_mgr.play("sin 4");
  std::this_thread::sleep_for(1s);
} // test_mix

void test_play_after() {
  std::cout << "\nAudio test: playing after" << std::endl;
  auto track_1 = make_sin_wave(0.007);
  auto track_2 = make_sin_wave(0.014);

  Sound_mgr sound_mgr;
  sound_mgr.move_audio("sin 1", std::move(track_1));
  sound_mgr.move_audio("sin 2", std::move(track_2));

  Audio_id id_1 = sound_mgr.play("sin 1");
  Audio_id id_2 = BAD_AUDIO;
  while (true) {
    if (!sound_mgr.is_playing(id_1) && id_2 == BAD_AUDIO) {
      id_2 = sound_mgr.play("sin 2");
      id_1 = BAD_AUDIO;
    }
    break_if (!sound_mgr.is_playing(id_2) && id_1 == BAD_AUDIO);
    std::this_thread::yield();
  }
} // test_play_after

void test_overplay() {
  std::cout << "\nAudio test: overplay" << std::endl;

  // добавить звук в базу
  Sound_mgr sound_mgr;
  auto sine_wave = make_sin_wave(2.0);
  sound_mgr.move_audio("sin", std::move(sine_wave));
  sound_mgr.set_listener_pos(Vec3(0, 0, 1));

  // проиграть звук
  cauto rand_pos = []->float {
    return ((rand() / float(RAND_MAX)) * 2.0 - 1.0) * 15.0; };
  cfor (i, 256) {
    Vec3 pos(rand_pos(), rand_pos(), rand_pos());
    sound_mgr.play("sin", pos, {}, 0.3);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms);
  }
} // test_overplay

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
  test_noise();
  test_motion_sine();
  test_mix();
  test_play_after();
  test_overplay();
  //test_file();
}
