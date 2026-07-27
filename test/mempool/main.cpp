#undef NDEBUG
#include <cassert>
#include <array>
#include "util/mempool.hpp"
#include "util/log.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

struct Dummy {
  uint val_0 {};
  char data [64];
  uint val_1 {};
  uint val_2 {};
};

int main() {
  log_info << ("mem pool tests start\n");

  { // создание пулов
    Mem_pool pool_0;
    Mem_pool pool_1(1024*1024);
    Mem_pool pool_2(1024*1024*1024);
  }
  { // выделение памяти в пустом пуле
    Mem_pool pool(0);
    auto o1 = pool.new_object<Dummy>();
    auto o2 = pool.new_object<Dummy>();
    auto o3 = pool.new_object<Dummy>();
    o1->val_2 = 3;
    o2->val_2 = 2;
    o3->val_2 = 1;
    assert(o1);
    assert(o2);
    assert(o3);
    assert(o1->val_2);
    assert(o2->val_2);
    assert(o3->val_2);
  }
  { // переполнение пула
    Mem_pool pool(sizeof(Dummy) * 2);

    auto a = pool.new_object<Dummy>();
    a->val_0 = 20;
    a->val_1 = 30;
    a->val_2 = 40;

    std::array<Dummy*, 100> table;
    for (uint idx = 0; rauto it: table) {
      it = pool.new_object<Dummy>();
      it->val_1 = idx++;
    }
    for (uint idx = 0; crauto it: table) {
      assert(it->val_1 == idx);
      ++idx;
    }

    assert(a->val_0 == 20);
    assert(a->val_1 == 30);
    assert(a->val_2 == 40);
  }
  { // выделение шаред памяти в пустом пуле
    Mem_pool pool(0);
    auto o1 = pool.new_object_shared<Dummy>();
    auto o2 = pool.new_object_shared<Dummy>();
    auto o3 = pool.new_object_shared<Dummy>();
    o1->val_2 = 3;
    o2->val_2 = 2;
    o3->val_2 = 1;
    assert(o1);
    assert(o2);
    assert(o3);
    assert(o1->val_2);
    assert(o2->val_2);
    assert(o3->val_2);
  }
  { // переполнение пула c шаред объектами
    Mem_pool pool(sizeof(Dummy) * 2);

    auto a = pool.new_object_shared<Dummy>();
    a->val_0 = 20;
    a->val_1 = 30;
    a->val_2 = 40;

    std::array<Shared<Dummy>, 100> table;
    for (uint idx = 0; rauto it: table) {
      it = pool.new_object_shared<Dummy>();
      it->val_1 = idx++;
    }
    for (uint idx = 0; crauto it: table) {
      assert(it->val_1 == idx);
      ++idx;
    }

    assert(a->val_0 == 20);
    assert(a->val_1 == 30);
    assert(a->val_2 == 40);
  }
  { // ресет шаред памяти
    Mem_pool pool;
    auto x = pool.new_object_shared<Dummy>();
    x->val_0 = 10;
    x->val_1 = 20;
    x->val_2 = 30;
    pool.release();
    assert( !x);

    x = pool.new_object_shared<Dummy>();
    assert(x);
    x->val_0 = 10;
    x->val_1 = 20;
    x->val_2 = 30;
    assert(x->val_0 == 10);
    assert(x->val_1 == 20);
    assert(x->val_2 == 30);
  }

  log_info << ("mem pool tests end\n");
}
