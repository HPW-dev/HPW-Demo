#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#ifdef CLD_DEBUG
#include <atomic>
#endif
#include "graphic/image/image-fwd.hpp"

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
inline bool show_fps           {}; // отображать инфу о dt и fps
inline bool show_grids         {}; // сетка для разбиения пространства
inline bool draw_hitboxes      {}; // показать хитбокс
inline bool draw_plots         {}; // игровые данные в виде графиков
inline bool draw_full_hitboxes {}; // показать ещё и покрывающий круг с крестом
inline bool draw_entity_pos    {};
inline bool draw_entities      {true};
inline bool draw_level         {true}; // рисовать фон уровня
inline bool show_virtual_joystick      {}; // показать что за кнопки нажимаются
inline bool draw_entity_hp     {}; // показать жизни у объекта
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
