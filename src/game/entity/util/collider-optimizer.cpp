#include <functional>
#include "collider-optimizer.hpp"
#include "util/str.hpp"
#include "util/math/average.hpp"
#include "game/core/core.hpp"
#include "game/core/common.hpp"
#include "game/core/graphic.hpp"
#include "game/core/canvas.hpp"
#include "game/entity/entity-mgr.hpp"
#include "game/entity/collider/collider-2d-tree.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/entity/collider/collider-grid.hpp"
#include "game/entity/collider/collider-simple.hpp"

using Maker = std::function< Shared<Collider> () >;
constexpr std::size_t SAVE_SAMPLES_DELAY = 5; // сколько времени сохранять статистику по тормозам

struct Collider_info {
  Str name {};
  Maker maker {};
  Average<Delta_time, SAVE_SAMPLES_DELAY> avr_tick_time {};
};

struct Collider_optimizer::Impl {
  Entity_mgr& m_entity_mgr;
  constx uint NEXT_SORT_DELAY = 240 * 1.5; // через сколько тиков можно продолжить замеры
  using Collider_infos = Vector<Collider_info>;
  Collider_infos m_collider_infos {};
  std::size_t m_cur_collider {999'999'999}; // индекс для таблицы из m_collider_infos
  uint m_sort_delay {};
  uint m_save_samples_delay {SAVE_SAMPLES_DELAY};

  explicit inline Impl(Entity_mgr& entity_mgr)
  : m_entity_mgr {entity_mgr}
  {
    m_collider_infos = {
      #define INFO(TITLE, CLASS, ...) \
      Collider_info { \
        .name = TITLE, \
        .maker = []{ return new_shared<CLASS>(__VA_ARGS__); } \
      },

      INFO("2d-tree", Collider_2d_tree)
      INFO("q-tree", Collider_qtree, 7, 1, graphic::width, graphic::height)
      INFO("grid", Collider_grid)
      INFO("simple", Collider_simple)

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
    return_if (m_entity_mgr.collider_time() < hpw::target_update_time * 1.1);

    // если первый раз
    if (m_cur_collider >= 999'999'999) {
      m_cur_collider = 0;
      set_collider(m_cur_collider);
    }

    // сохранить статы для текущего коллайдера из списка
    if (m_save_samples_delay) {
      cauto collider_time = m_entity_mgr.collider_time();
      m_collider_infos.at(m_cur_collider).avr_tick_time.push(collider_time);
      --m_save_samples_delay;
    } else {
      m_save_samples_delay = SAVE_SAMPLES_DELAY;

      ++m_cur_collider;
      if (m_cur_collider >= m_collider_infos.size()) {
        // выбрать самый оптимальный коллайдер из списка и уйти в сон
        m_sort_delay = NEXT_SORT_DELAY;
        m_entity_mgr.set_collider( fastest_collider() );
        m_cur_collider = 0;
      }

      // переключиться на следующий коллайдер
      set_collider(m_cur_collider);
    }
  } // update

  inline Shared<Collider> fastest_collider() const {
    std::size_t idx {};
    for (std::size_t i = 1; i < m_collider_infos.size(); ++i)
      if (m_collider_infos.at(idx).avr_tick_time() > m_collider_infos.at(i).avr_tick_time())
        idx = i;
    return m_collider_infos.at(idx).maker();
  }

  inline void set_collider(std::size_t collider_idx) {
    cnauto selected = m_collider_infos.at(collider_idx);
    m_entity_mgr.set_collider(selected.maker());
  }
}; // Impl

Collider_optimizer::Collider_optimizer(Entity_mgr& entity_mgr): impl{new_unique<Impl>(entity_mgr)} {}
Collider_optimizer::~Collider_optimizer() {}
void Collider_optimizer::update() { impl->update(); }
