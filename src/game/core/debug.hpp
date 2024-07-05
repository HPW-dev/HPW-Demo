#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#ifdef CLD_DEBUG
#include <atomic>
#endif

class Image;

namespace hpw {
#ifdef CLD_DEBUG
  inline std::atomic_uint64_t circle_checks {}; // сколько проверок круга за тик
  inline std::atomic_uint64_t poly_checks {}; // сколько проверок полигонов за тик
  inline std::atomic_uint64_t total_collided {}; // сколько за тик столкнулось объектов
#endif

inline Shared<Image> hitbox_layer {}; // для рендера хитбоксов поверх канваса
inline bool update_delay {false}; // искусственная задержка игры
inline bool render_delay {false}; // искусственная задержка отрисовки
inline bool show_entity_mem_map {false};
/** когда включён, можно юзать стабильные рандомы
@details нужен чтобы выявить места, где нельзя вызывать такие рандомы */
inline bool allow_random_stable {false};
inline bool allow_random_stable_bak {false};
inline Str start_script {}; // выполнить этот скрипт при старте игры
inline bool empty_level_first {}; // запустить при старте игры пустой уровень
} // hpw ns

namespace graphic {
inline bool show_debug_info     {false}; // отображать инфу о dt и fps
inline bool show_grids          {false}; // сетка для разбиения пространства
inline bool draw_hitboxes       {false}; // показать хитбокс
inline bool draw_full_hitboxes  {false}; // показать ещё и покрывающий круг с крестом
inline bool draw_entity_pos     {false};
inline bool draw_entities        {true};
inline bool draw_level          {true};  // рисовать фон уровня
inline bool draw_controls       {false}; // показать что за кнопки нажимаются
inline bool draw_entity_hp      {false}; // показать жизни у объекта
}

#ifdef STABLE_REPLAY
  #define ALLOW_STABLE_RAND Scope replay_stable_scope ( \
    [] { \
      hpw::allow_random_stable_bak = hpw::allow_random_stable; \
      hpw::allow_random_stable = true; \
    }, \
    [] { \
      hpw::allow_random_stable = hpw::allow_random_stable_bak; \
    } \
  );
#else
  #define ALLOW_STABLE_RAND
#endif
