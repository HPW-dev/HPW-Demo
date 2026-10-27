#include "pch.hpp"
#include "scene-random-test.hpp"
#include "game/core/messages.hpp"
#include "game/core/sprites.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/input/keys.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "engine/graphic/util/resize.hpp"

class Rnd_test_base {
public:
  virtual ~Rnd_test_base() = default;
  virtual void update(Rnd_gen_base* rnd_gen, uint samples) = 0;
  virtual void draw(Image& dst) const = 0;
};

// просто квадраты с шумом
class Noise_squares final: public Rnd_test_base {
  Image _map1, _map2, _map3, _map4;
  std::bernoulli_distribution _dist{0.5};
  std::uniform_int_distribution<u32_t> _dist2{0, 255};
  std::uniform_real_distribution<real> _distf{0, 1};

public:
  sconst uint SZ = 96;
  sconst uint offx = 17;
  sconst uint offy = offx;
  sconst uint spacex = 30;

  inline explicit Noise_squares()
  : _map1(SZ, SZ)
  , _map2(SZ, SZ)
  , _map3(SZ, SZ)
  , _map4(SZ, SZ)
  {}

  inline void update(Rnd_gen_base* rnd_gen, uint _) override {
    assert(rnd_gen);
    for (rauto c: _map1)
      c = _dist(*rnd_gen) ? Pal8::white : Pal8::black;
    for (rauto c: _map2)
      c = _dist2(*rnd_gen);
    for (rauto c: _map3)
      c = Pal8::from_real(_distf(*rnd_gen));
    for (rauto c: _map4)
      c = Pal8::from_real(_distf(*rnd_gen), true);
  }

  inline void draw(Image& dst) const override {
    insert(dst, _map1, {offx, offy});
    insert(dst, _map2, {offx + SZ + spacex, offy});
    insert(dst, _map3, {offx + (SZ + spacex) * 2, offy});
    insert(dst, _map4, {offx + (SZ + spacex) * 3, offy});

    graphic::font->draw(dst, {17, dst.Y - 24},
      U"[ Квадраты со случайными цветами ]");
  }
};

// Рисует график распределения самых частых цифр
class Distribution final: public Rnd_test_base {
  sconst int offx = 17;
  sconst int offy = offx;
  sconst uint WIDTH = 512 - offx * 2;
  sconst uint HEIGHT = 150;
  Vector<uint> _dist_map;
  std::uniform_int_distribution<uint> _dist{0, WIDTH-1};
  uint _maximum; // поможет подгонать по высоте

public:
  explicit Distribution(): _dist_map(WIDTH) {}

  void update(Rnd_gen_base* rnd_gen, uint samples) override {
    assert(rnd_gen);
    assert(!_dist_map.empty());

    std::fill(_dist_map.begin(), _dist_map.end(), 0);
    _maximum = 0;

    cfor (_, samples) {
      cauto idx = _dist(*rnd_gen);
      auto& count = _dist_map.at(idx);
      ++count;
      _maximum = std::max(count, _maximum);
    }
  }

  void draw(Image& dst) const override {
    const Recti rect(offx, offy, WIDTH, HEIGHT);
    draw_rect(dst, rect, Pal8::white);

    ret_if(_maximum == 0);
    cauto HMUL = 1.0 / scast<double>(_maximum);

    cfor (x, _dist_map.size()) {
      cauto height = _dist_map[x] * HMUL * HEIGHT;
      cfor(y, height)
        dst.set(offx + x, offy + HEIGHT - 1 - y, Pal8::white, {});
    }

    graphic::font->draw(dst, {17, dst.Y - 24},
      U"[ Распределение чисел ]");
  }
};

struct Scene_random_test::Impl {
  Vector<Unique<Rnd_test_base>> _tests {};
  Rnd_test_base* _cur_test {};
  size_t _test_idx {0};

  Unique<Rnd_gen_base> _rnd_gen {};
  size_t _rnd_gen_idx {0};

  uint _samples;
  size_t _samples_idx {2};

  inline Impl() {
    _tests.emplace_back(new_unique<Distribution>());
    _tests.emplace_back(new_unique<Noise_squares>());
    set_samples(0);
    set_test(0);
    set_gen(0);

    print_inputs();
  }

  inline void print_inputs() {
    hpw::message_mgr->move(Message{
      .text=U"Управление:\n  "
        + get_key_name(hpw::keycode::up) + U"/"
        + get_key_name(hpw::keycode::down) + U" - Сменить генератор рандома.\n  "
        + get_key_name(hpw::keycode::left) + U"/"
        + get_key_name(hpw::keycode::right) + U" - Следующий тест.\n  "
        + get_key_name(hpw::keycode::focus) + U" + "
        + get_key_name(hpw::keycode::up) + U"/"
        + get_key_name(hpw::keycode::down) + U" - Изменить размер выборки.\n  ",
      .bf = &blend_diff
    });
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape)) hpw::scene_mgr.back();
    if (is_pressed_once(hpw::keycode::left))  set_test(-1);
    if (is_pressed_once(hpw::keycode::right)) set_test(+1);

    if (is_pressed(hpw::keycode::focus)) {
      if (is_pressed_once(hpw::keycode::up))   set_samples(+1);
      if (is_pressed_once(hpw::keycode::down)) set_samples(-1);
    } else {
      if (is_pressed_once(hpw::keycode::up))   set_gen(-1);
      if (is_pressed_once(hpw::keycode::down)) set_gen(+1); 
    }

    assert(_cur_test);
    assert(_rnd_gen);
    _cur_test->update(_rnd_gen.get(), _samples);
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    
    assert(_cur_test);
    _cur_test->draw(dst);
  }

  inline void set_gen(int idx_offset) {
    cauto gens = rnd_gens();
    assert(!gens.empty());
    int idx = _rnd_gen_idx + idx_offset;
    if (idx < 0)
      idx += gens.size();
    _rnd_gen_idx = idx % gens.size();
    cauto name = gens.at(_rnd_gen_idx);
    _rnd_gen = make_rnd_gen(name);

    hpw::message_mgr->move(Message{
      .text=U"Генератор: " + utf8_to_32(name), .bf=&blend_diff });
  }

  inline void set_test(int idx_offset) {
    assert(!_tests.empty());
    int idx = _test_idx + idx_offset;
    if (idx < 0)
      idx += _tests.size();
    _test_idx = idx % _tests.size();
    _cur_test = _tests.at(_test_idx).get();

    hpw::message_mgr->move(Message{
      .text=U"Тест №: " + n2s<utf32>(_test_idx), .bf=&blend_diff });
  }

  inline void set_samples(int idx_offset) {
    sconst Vector<uint> table {
      100,
      1000,
      4096,
      10'000,
      50'000,
      200'000,
      1'000'000,
    };
    int idx = _samples_idx + idx_offset;
    if (idx < 0)
      idx += table.size();
    _samples_idx = idx % table.size();
    _samples = table.at(_samples_idx);

    hpw::message_mgr->move(Message{
      .text=U"Выборка: " + n2s<utf32>(_samples), .bf=&blend_diff });
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_random_test)
