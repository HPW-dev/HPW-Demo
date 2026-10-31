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


static utf32 g_gen_name {};

class Rnd_test_base {
public:
  virtual ~Rnd_test_base() = default;
  virtual void update(Stl_rnd& rnd_gen, uint samples) = 0;
  virtual void draw(Image& dst) const = 0;
};

// вероятности как в покере
class Poker: public Rnd_test_base {
public:
  struct Card {
    int rang;
    int suit; // масть
  };
  
  struct Stats {
    size_t royal_flush {};
    size_t straight_flush {};
    size_t four_of_a_kind {}; // каре
    size_t full_house {};
    size_t flush {};
    size_t straight {};
    size_t three_of_a_kind {};
    size_t two_pair {};
    size_t one_pair {};
    size_t high_card {};
  };

  // вероятности выпадения разных комбинаций
  staticx double chance_royal_flush      = 0.00000153907716933;
  staticx double chance_straight_flush   = 0.00001385169452396;
  staticx double chance_four_of_a_kind   = 0.00024009603841537;
  staticx double chance_full_house       = 0.00144057623049220;
  staticx double chance_flush            = 0.00196540154523348;
  staticx double chance_straight         = 0.00392464678178964;
  staticx double chance_three_of_a_kind  = 0.02112845138055222;
  staticx double chance_two_pair         = 0.04753901560624250;
  staticx double chance_one_pair         = 0.42256902761104442;
  staticx double chance_high_card        = 0.50117739403453687;

public:
  Poker() {
    cfor (s, 4)
    cfor (r, 13) {
      Card card;
      card.rang = r;
      card.suit = s;
      _cards.emplace_back(card);
    }
  };

  void update(Stl_rnd& rnd_gen, uint samples) override {
    // чекаем на свич генератора
    auto gen_ptr = rnd_gen.gen.get();
    if (_prev_gen != gen_ptr) {
      _prev_gen = gen_ptr;
      _rounds = 0;
      _stats = {};
    }

    cfor (_, samples) {
      std::shuffle(_cards.begin(), _cards.end(), rnd_gen);
      ++_rounds;
      calculate_stats();
    }
  }

  void draw(Image& dst) const override {
    ret_if (_rounds == 0);
    ret_if (!_prev_gen);

    utf32 stats_text;
    const double rounds_real = _rounds;
    const uint P = 7;
    cauto stat_royal_flush = _stats.royal_flush / rounds_real;
    cauto stat_straight_flush = _stats.straight_flush / rounds_real;
    cauto stat_four_of_a_kind = _stats.four_of_a_kind / rounds_real;
    cauto stat_full_house = _stats.full_house / rounds_real;
    cauto stat_flush = _stats.flush / rounds_real;
    cauto stat_straight = _stats.straight / rounds_real;
    cauto stat_three_of_a_kind = _stats.three_of_a_kind / rounds_real;
    cauto stat_two_pair = _stats.two_pair / rounds_real;
    cauto stat_one_pair = _stats.one_pair / rounds_real;
    cauto stat_high_card = _stats.high_card / rounds_real;
    cauto total_error =
      std::abs(chance_royal_flush - stat_royal_flush) +
      std::abs(chance_straight_flush - stat_straight_flush) +
      std::abs(chance_four_of_a_kind - stat_four_of_a_kind) +
      std::abs(chance_full_house - stat_full_house) +
      std::abs(chance_flush - stat_flush) +
      std::abs(chance_straight - stat_straight) +
      std::abs(chance_three_of_a_kind - stat_three_of_a_kind) +
      std::abs(chance_two_pair - stat_two_pair) +
      std::abs(chance_one_pair - stat_one_pair) +
      std::abs(chance_high_card - stat_high_card);
    stats_text = U"Статистика по комбинациям:\n"
      U"Royal flush ...... " + n2s<utf32>(stat_royal_flush, P)     + U"/" + n2s<utf32>(chance_royal_flush, P) + U"\n"
      U"Straight flush ... " + n2s<utf32>(stat_straight_flush, P)  + U"/" + n2s<utf32>(chance_straight_flush, P) + U"\n"
      U"Four of a kind ... " + n2s<utf32>(stat_four_of_a_kind, P)  + U"/" + n2s<utf32>(chance_four_of_a_kind, P) + U"\n"
      U"Full house ....... " + n2s<utf32>(stat_full_house, P)      + U"/" + n2s<utf32>(chance_full_house, P) + U"\n"
      U"Flush ............ " + n2s<utf32>(stat_flush, P)           + U"/" + n2s<utf32>(chance_flush, P) + U"\n"
      U"Straight ......... " + n2s<utf32>(stat_straight, P)        + U"/" + n2s<utf32>(chance_straight, P) + U"\n"
      U"Three of a kind .. " + n2s<utf32>(stat_three_of_a_kind, P) + U"/" + n2s<utf32>(chance_three_of_a_kind, P) + U"\n"
      U"Two pair ......... " + n2s<utf32>(stat_two_pair, P)        + U"/" + n2s<utf32>(chance_two_pair, P) + U"\n"
      U"One pair ......... " + n2s<utf32>(stat_one_pair, P)        + U"/" + n2s<utf32>(chance_one_pair, P) + U"\n"
      U"High card ........ " + n2s<utf32>(stat_high_card, P)       + U"/" + n2s<utf32>(chance_high_card, P) + U"\n"
      U"\n"
      U"Раундов: " + n2s<utf32>(_rounds) + U"\n"
      U"Отклонение: " + n2s<utf32>(total_error, P) + U"\n"
      U"Генератор: " + g_gen_name + U"\n"
    ;
    graphic::system_mono->draw(dst, {4, 15}, stats_text);

    graphic::font->draw(dst, {17, dst.Y - 24}, U"[ Вероятность выпадения карт в Покере ]");
  };

private:
  Vector<Card> _cards;
  Stats _stats;
  size_t _rounds {};
  Rnd_gen_base* _prev_gen {}; // если поменяли генератор, сбрасываем статистику

  void calculate_stats() {
    // Берем первые 5 карт из перемешанной колоды
    std::array<Card, 5> hand;
    for (size_t i = 0; i < 5; ++i) {
      hand[i] = _cards[i];
    }

    // Сортируем карты по возрастанию ранга (от 0 до 12)
    std::sort(hand.begin(), hand.end(), [](const Card& a, const Card& b) {
      return a.rang < b.rang;
    });

    // 1. Проверка на Флеш (все карты одной масти)
    const bool is_flush = (hand[0].suit == hand[1].suit &&
                           hand[0].suit == hand[2].suit &&
                           hand[0].suit == hand[3].suit &&
                           hand[0].suit == hand[4].suit);

    // 2. Проверка на Стрит (последовательные ранги)
    // Обычный стрит: каждый следующий ранг строго на 1 больше предыдущего
    const bool is_regular_straight = (hand[1].rang == hand[0].rang + 1 &&
                                      hand[2].rang == hand[0].rang + 2 &&
                                      hand[3].rang == hand[0].rang + 3 &&
                                      hand[4].rang == hand[0].rang + 4);

    // Стрит от Туза (Колесо / Колесообразный стрит): ранги 0, 1, 2, 3 и 12 (Туз)
    const bool is_ace_low_straight = (hand[0].rang == 0 && 
                                      hand[1].rang == 1 && 
                                      hand[2].rang == 2 && 
                                      hand[3].rang == 3 && 
                                      hand[4].rang == 12);

    const bool is_straight = is_regular_straight || is_ace_low_straight;

    // 3. Подсчет одинаковых рангов (для пар, троек, каре и фулл-хаусов)
    // Так как рука отсортирована, одинаковые ранги идут строго друг за другом
    int pairs_count = 0;
    int trips_count = 0;
    int quads_count = 0;

    // Считаем вхождения каждого номинала прямо по сортированной руке
    for (size_t i = 0; i < 5; ) {
      size_t j = i + 1;
      while (j < 5 && hand[j].rang == hand[i].rang) {
        ++j;
      }
      size_t count = j - i;
      if (count == 4) quads_count++;
      elif (count == 3) trips_count++;
      elif (count == 2) pairs_count++;
      i = j;
    }

    // 4. Определение комбинации по иерархии (от старшей к младшей)
    if (is_flush && is_straight) {
      // Роял-флеш — это обычный стрит-флеш, который заканчивается Тузом (ранг 12).
      // Важно: стрит-флеш А-2-3-4-5 заканчивается на Туза, но Роял-флешем не является.
      if (hand[4].rang == 12 && !is_ace_low_straight) {
        _stats.royal_flush++;
      } else {
        _stats.straight_flush++;
      }
    } 
    elif (quads_count == 1) {
      _stats.four_of_a_kind++;
    } 
    elif (trips_count == 1 && pairs_count == 1) {
      _stats.full_house++;
    } 
    elif (is_flush) {
      _stats.flush++;
    } 
    elif (is_straight) {
      _stats.straight++;
    } 
    elif (trips_count == 1) {
      _stats.three_of_a_kind++;
    } 
    elif (pairs_count == 2) {
      _stats.two_pair++;
    } 
    elif (pairs_count == 1) {
      _stats.one_pair++;
    } 
    else {
      _stats.high_card++;
    }
  }
}; // Poker

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

  inline void update(Stl_rnd& rnd_gen, uint _) override {
    for (rauto c: _map1)
      c = _dist(rnd_gen) ? Pal8::white : Pal8::black;
    for (rauto c: _map2)
      c = _dist2(rnd_gen);
    for (rauto c: _map3)
      c = Pal8::from_real(_distf(rnd_gen));
    for (rauto c: _map4)
      c = Pal8::from_real(_distf(rnd_gen), true);
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

  void update(Stl_rnd& rnd_gen, uint samples) override {
    assert(!_dist_map.empty());

    std::fill(_dist_map.begin(), _dist_map.end(), 0);
    _maximum = 0;

    cfor (_, samples) {
      cauto idx = _dist(rnd_gen);
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

  Stl_rnd _rnd_gen {};
  size_t _rnd_gen_idx {0};

  uint _samples;
  size_t _samples_idx {2};

  inline Impl() {
    _tests.emplace_back(new_unique<Poker>());
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
    _cur_test->update(_rnd_gen, _samples);
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
    g_gen_name = utf8_to_32(name);
    _rnd_gen = std::move(make_rnd_gen(name));

    hpw::message_mgr->move(Message{
      .text=U"Генератор: " + g_gen_name, .bf=&blend_diff });
  }

  inline void set_test(int idx_offset) {
    assert(!_tests.empty());
    int idx = _test_idx + idx_offset;
    if (idx < 0)
      idx += _tests.size();
    _test_idx = idx % _tests.size();
    _cur_test = _tests.at(_test_idx).get();

    hpw::message_mgr->move(Message{
      .text=U"Тест №: " + n2s<utf32>(_test_idx+1), .bf=&blend_diff });
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
