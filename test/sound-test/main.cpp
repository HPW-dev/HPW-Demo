#include <cstring>
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/path.hpp"
#include "util/vector-types.hpp"
#include "util/delay-for.hpp"
#include "game/util/resource.hpp"
#include "game/util/resource-helper.hpp"
#include "game/core/common.hpp"
#include "sound/sound.hpp"

void test_init_status() {
  log_info << "::: Audio test ::: check empty status :::";

  // проверить что звук инициализирован и ничего не проигрывается
  sound::init();
  cauto info = sound::info();
  iferror(!info.enabled, "sound is not enabled");
  iferror(info.tracks_playing_now != 0, "tracks running now != 0");
}

void test_bad_name() {
  log_info << "::: Audio test ::: check bad track name :::";
  Shared<sound::Track> track {};
  bool passed {false};

  // проверить создание несуществующего трека и реакции на это
  try {
    track = sound::play("unknown name test.wav");
  } catch (cr<hpw::Error> err) {
    log_info << "planned error: " << err.get_msg();
    passed = true;
  } catch (...) {
    log_warning << "unknown error";
    passed = true;
  }

  if (passed) {
    iferror(track, "track is not empty");
    iferror(sound::info().total_played != 0, "total_played != 0");
    iferror(sound::info().tracks_playing_now != 0, "tracks_playing_now != 0");
    log_info << "passed";
    return;
  }

  error("it was impossible to get into this part of code");
}

sound::Buffer make_sin_wave(float freq) {
  sound::Buffer ret;
  ret.channels = 1;
  ret.compression = sound::Compression::raw;
  ret.format = sound::Format::f32;
  ret.frequency = 48'000;
  ret.samples = ret.frequency * 4;
  ret.file.set_path("sine wave (freq " + n2s(freq, 3) + ").mono_pcm_f32");
  Vector<float> f32_wave(ret.samples * ret.channels);
  float step {};

  for (rauto sample: f32_wave) {
    sample = std::sin(step);
    step += freq;
  }

  ret.file.data.resize(ret.samples * sizeof(float) * ret.channels);
  std::memcpy(ret.file.data.data(), f32_wave.data(), ret.file.data.size());
  return ret;
}

void test_sine_1() {
  log_info << "::: Audio test ::: playing sine wave (1) :::";
  auto buf = make_sin_wave(0.15);
  sound::play(buf);
  iferror(sound::info().tracks_playing_now != 1, "tracks_playing_now != 1");

  // ожидание пока все звуки завершатся
  if (!wait_for([&]{ return sound::info().tracks_playing_now == 0; }, 15))
    log_error << "timeout!";
}

/*

void test_motion_sine() {
  log_info << "::: Audio test ::: motion sine wave :::";

  // добавить звук в базу
  Sound_mgr_oal sound_mgr;
  auto sine_wave = make_sin_wave(0.02);
  sound_mgr.move_audio("sine wave", std::move(sine_wave));
  sound_mgr.set_listener_pos(Vec3(0, 0, 1));
  sound_mgr.set_doppler_factor(20);

  // проиграть звук
  Vec3 pos(-25, 0, 0);
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
  log_info << "::: Audio test ::: playing noise :::";

  // сделать стерео шум
  Audio noise;
  noise.channels = 2;
  noise.compression = Audio::Compression::raw;
  noise.format = Audio::Format::raw_pcm_s16;
  noise.set_generated(true);
  noise.frequency = 22'100;
  noise.samples = noise.frequency * 3;
  noise.data.resize(noise.samples * sizeof(std::int16_t) * noise.channels);
  for (rauto sample: noise.data)
    sample = rand() % 256;
  
  // добавить звук в базу
  Sound_mgr_oal sound_mgr;
  sound_mgr.move_audio("noise", std::move(noise));

  // проиграть звук
  cauto audio_id = sound_mgr.play("noise", {}, {}, 0.01);
  wait_for([&]{ return !sound_mgr.is_playing(audio_id); }, 15);
} // test_sine

void test_mix() {
  log_info << "::: Audio test ::: mixing audio :::";
  auto track_1 = make_sin_wave(0.007);
  auto track_2 = make_sin_wave(0.01);
  auto track_3 = make_sin_wave(0.05);
  auto track_4 = make_sin_wave(0.1);
  Sound_mgr_oal sound_mgr;
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
  log_info << "::: Audio test ::: playing after :::";
  auto track_1 = make_sin_wave(0.007);
  auto track_2 = make_sin_wave(0.014);

  Sound_mgr_oal sound_mgr;
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
  log_info << "::: Audio test ::: overplay :::";

  // добавить звук в базу
  Sound_mgr_oal sound_mgr;
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

void test_file(cr<Str> fname) {
  log_info << "::: Audio test ::: loading file \"" << hpw::cur_dir + fname << "\" :::";
  cauto track = load_audio(fname);
  Sound_mgr_oal sound_mgr;
  sound_mgr.add_audio("music test", track);
  cauto audio_id = sound_mgr.play("music test");
  iferror(audio_id == BAD_AUDIO, "Bad audio ID");
  wait_for([&]{ return !sound_mgr.is_playing(audio_id); }, 4);
} // test_file
*/

int main(int argc, char *argv[]) {
  try {
    hpw::Logger::config.use_stream_debug = true;
    hpw::cur_dir = launch_dir_from_argv0(argv);

    test_init_status();
    test_bad_name();
    test_sine_1();
    /*test_noise();
    test_motion_sine();
    test_mix();
    test_play_after();
    test_overplay();
    test_file("resource/audio/sfx/recharge/recharge.flac");
    test_file("resource/audio/sfx/recharge/recharge fast 2.flac");*/
    log_info << "end of tests";
  } catch (cr<hpw::Error> err) {
    hpw::Logger::config.print_source = false;
    log_error << "audio test failed in: " << err.what();
  } catch (...) {
    hpw::Logger::config.print_source = false;
    log_error << "test failed: unknown error";
  }
}
