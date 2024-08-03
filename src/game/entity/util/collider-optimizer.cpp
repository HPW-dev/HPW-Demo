#include <functional>
#include "collider-optimizer.hpp"
#include "util/str.hpp"
#include "util/math/average.hpp"
#include "util/log.hpp"
#include "game/core/common.hpp"
#include "game/core/graphic.hpp"
#include "game/core/canvas.hpp"
#include "game/entity/entity-mgr.hpp"
#include "game/entity/collider/collider-2d-tree.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/entity/collider/collider-grid.hpp"
#include "game/entity/collider/collider-simple.hpp"

using Maker = std::function< Shared<Collider> () >;

struct Collider_info {
  Str name {};
  Maker maker {};
  Average<Delta_time, 30> avr_tick_time {};
};

struct Collider_optimizer::Impl {
  Entity_mgr& m_entity_mgr;
  constx uint NEXT_SORT_DELAY = 240 * 4; // через сколько тиков можно продолжить замеры
  using Collider_infos = Vector<Collider_info>;
  Collider_infos m_collider_infos {};
  std::size_t m_cur_collider {}; // индекс для таблицы из m_collider_infos
  uint m_sort_delay {};

  explicit inline Impl(Entity_mgr& entity_mgr)
  : m_entity_mgr {entity_mgr}
  {
    m_collider_infos = {
      #define INFO(TITLE, CLASS, ...) \
      Collider_info { \
        .name = TITLE, \
        .maker = []{ return new_shared<CLASS>(__VA_ARGS__); } \
      },

      INFO("simple", Collider_simple)
      INFO("2d-tree", Collider_2d_tree)
      INFO("grid", Collider_grid)
      INFO("q-tree", Collider_qtree, 7, 1, graphic::width, graphic::height)

      #undef INFO
    }; // m_entity_mgr
  } // Impl
  
  inline void update() {
    return_if (!hpw::collider_autoopt);

    /* При лагах начать перебирать колайдеры и замерять их время работы,
    если оно удовлетворяет, то оставить коллайдер, если нет, то двигаться дальше
    по списку. Если не удалось найти хороший коллайдер, выбрать вариант с самым
    хорошим временем и добавить задержку ожидания для следующих замеров */

    // кулдаун после прошлых замеров
    if (m_sort_delay) {
      --m_sort_delay;
      return;
    }

    // если ничего не леагает, то и не надо
    return_if (!m_entity_mgr.update_lag());

    // TODO
  } // update
}; // Impl

Collider_optimizer::Collider_optimizer(Entity_mgr& entity_mgr): impl{new_unique<Impl>(entity_mgr)} {}
Collider_optimizer::~Collider_optimizer() {}
void Collider_optimizer::update() { impl->update(); }
