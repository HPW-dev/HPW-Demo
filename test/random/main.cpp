#include <omp.h>
#include <functional>
#include <array>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "util/math/random.hpp"
#include "util/macro.hpp"
#include "util/error.hpp"

namespace {
  inline constexpr uint w {70};
  inline constexpr uint h {10};
  inline constexpr uint times {500'000};
  inline std::array<uint, h> table;
}

void print_table() {
  auto max_count {std::max_element(std::begin(table), std::end(table))};
  cfor (y, table.size()) {
    auto count {table.at(y)};
    uint len ((scast<double>(w) / *max_count) * count);
    cfor (x, len)
      std::cout << "#";
    cfor (x, w-len)
      std::cout << ".";
    std::cout << "\n";
  }
}

void visual_test(auto func) {
  table.fill(0);
  cfor (i, times)
    ++table.at(func());
  print_table();
}

void print_rnd_samples() {
  std::cout << "rndb: " << scast<int>(rndb()) << std::endl;
  std::cout << "rndb_fast: " << scast<int>(rndb_fast()) << std::endl;
  std::cout << "rnd: " << rnd() << std::endl;
  std::cout << "rnd_fast: " << rnd_fast() << std::endl;
  std::cout << "rnd_fast: " << rnd_fast() << std::endl;
  std::cout << "rndu: " << rndu() << std::endl;
  std::cout << "rndu_fast: " << rndu_fast() << std::endl;
  std::cout << "rndu_fast: " << rndu_fast() << std::endl;
  std::cout << "rndr: " << rndr() << std::endl;
  std::cout << "rndr_fast: " << rndr_fast() << std::endl;
  std::cout << "rndr_fast: " << rndr_fast() << std::endl;
}

// визуальный тест равномерности - значения при определённых сидах
void test_1() {
  std::cout << "test 1" << std::endl;
  set_rnd_seed(std::time({}));

  std::cout << "rand:\n"; visual_test([]{ return std::rand() % table.size(); });
  std::cout << "\nrnd:\n"; visual_test([]{ return rnd(0, table.size()-1); });
  std::cout << "\nrnd_fast:\n"; visual_test([]{ return rnd_fast() % table.size(); });
  std::cout << "\nrndb:\n"; visual_test([]{ return rndb() % table.size(); });
  std::cout << "\nrndb_fast:\n"; visual_test([]{ return rndb_fast() % table.size(); });
  std::cout << "\nrndu:\n"; visual_test([]{ return rndu(table.size()-1); });
  std::cout << "\nrndu_fast:\n"; visual_test([]{ return rndu_fast() % table.size(); });
  std::cout << "\nrndr:\n"; visual_test([]{ return rndr(0, 1) * (table.size()-1); });
  std::cout << "\nrndr_fast:\n"; visual_test([]{ return rndr_fast() * table.size(); });

  int seed = 1;
  std::cout << "\n\nseed: " << seed << std::endl;
  set_rnd_seed(seed);
  print_rnd_samples();

  seed = 97997;
  std::cout << "\nseed: " << seed << std::endl;
  set_rnd_seed(seed);
  print_rnd_samples();
} // test_1

struct Answers {
  decltype(rnd()) val_rnd {};
  decltype(rndu()) val_rndu {};
  decltype(rndb()) val_rndb {};
  decltype(rndr()) val_rndr {};

  inline bool operator ==(CN<Answers> other) const {
    if (val_rnd != other.val_rnd) { std::cout << "val_rnd != other.val_rnd" << std::endl; return false; }
    if (val_rndu != other.val_rndu) { std::cout << "val_rndu != other.val_rndu" << std::endl; return false; }
    if (val_rndb != other.val_rndb) { std::cout << "val_rndb != other.val_rndb" << std::endl; return false; }
    if (val_rndr != other.val_rndr) { std::cout << "val_rndr != other.val_rndr" << std::endl; return false; }
    return true;
  }
}; // Answers

Answers gen() {
  Answers answers {
    .val_rnd = rnd(),
    .val_rndu = rndu(),
    .val_rndb = rndb(),
    .val_rndr = rndr()
  };
  return answers;
} // gen

void print(CN<Answers> answers) {
  std::cout << "rnd:\t\t"  << +answers.val_rnd << std::endl;
  std::cout << "rndu:\t\t" << +answers.val_rndu << std::endl;
  std::cout << "rndb:\t\t" << +answers.val_rndb << std::endl;
  std::cout << "rndr:\t\t" << +answers.val_rndr << std::endl;
} // print

// тест совпадения значений
void test_2() {
  std::cout << "\nreset seed test" << std::endl;
  set_rnd_seed(1703101347u);
  auto a = gen();
  print(a);

  std::cout << "\nreset..." << std::endl;
  set_rnd_seed(1703101347u);
  auto b = gen();
  iferror(a != b, "reset seed test failed");
} // test_2

void multicore_test(Answers& before, Answers& after) {
  before = gen();
  #pragma omp parallel for
  cfor (_, 100'000u) {
    auto tmp = gen();
    do_not_optimize(tmp);
  }
  after = gen();
}

// проверяет совпадение начений при многопоточных вызовах
void test_3() {
  std::cout << "\nmultithread test" << std::endl;
  auto seed = time({});

  set_rnd_seed(seed);
  Answers before_1, after_1;
  multicore_test(before_1, after_1);
  print(after_1);

  std::cout << "\nrepeat..." << std::endl;
  set_rnd_seed(seed);
  Answers before_2, after_2;
  multicore_test(before_2, after_2);
  print(after_2);

  iferror(after_1 != after_2, "multithread test failed");
} // test_3

void test_4() {
  std::cout << "\ntest 3: file diff" << std::endl;
  std::ofstream file_1("delme 1.txt");
  std::ofstream file_2("delme 2.txt");

  set_rnd_seed(time({}));
  cfor (_, rnd(4000, 8000)) {
    auto tmp = gen();
    do_not_optimize(tmp);
  }

  set_rnd_seed(17'245'124u);
  cfor (i, 8000) {
    auto ret = gen();
    file_1 << i << ":\n";
    file_1 << ret.val_rnd << '\n';
    file_1 << +ret.val_rndb << '\n';
    file_1 << ret.val_rndr << '\n';
    file_1 << ret.val_rndu << '\n';
    file_1 << '\n';
  }
  file_1.close();

  cfor (_, rnd_fast(4000, 8000)) {
    auto tmp = gen();
    do_not_optimize(tmp);
  }

  set_rnd_seed(17'245'124u);
  cfor (i, 8000) {
    auto ret = gen();
    file_2 << i << ":\n";
    file_2 << ret.val_rnd << '\n';
    file_2 << +ret.val_rndb << '\n';
    file_2 << ret.val_rndr << '\n';
    file_2 << ret.val_rndu << '\n';
    file_2 << '\n';
  }
  file_2.close();

} // test 3

int main() {
  test_1();
  test_2();
  test_3();
  test_4();
  std::cout << "\nall tests complete" << std::endl;
} // main
