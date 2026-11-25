#include "pch.hpp"
#include "port-timer.hpp"

#ifdef WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else // LINUX
  #include <unistd.h>
  #include <climits>
#endif


using std_seconds = std::chrono::duration<double, std::ratio<1, 1>>;

inline void std_delay(const Seconds seconds) {
  auto st = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - st <= std_seconds(seconds))
    std::this_thread::yield();
}

inline void std_delay_nop(const Seconds seconds) {
  auto st = std::chrono::steady_clock::now();
  while (std::chrono::steady_clock::now() - st <= std_seconds(seconds)) {
    asm volatile ("");
  }
}

inline void std_sleep_until(const Seconds seconds)
  { std::this_thread::sleep_until(std::chrono::steady_clock::now() + std_seconds(seconds)); }

#ifdef WINDOWS

inline void win_sleep(const Seconds seconds) { ::Sleep(seconds * 1'000); }

inline void win_htimer(const Seconds seconds) {
#ifdef is_x32
  // TODO x32 версия таймера
  win_sleep(seconds);
#else // x64
  ::LARGE_INTEGER ft;
  ft.QuadPart = -static_cast<std::int64_t>(seconds * 10'000'000);  // '-' using relative time
  ::HANDLE timer = ::CreateWaitableTimerExW(NULL, NULL,
    CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
  ::SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
  ::WaitForSingleObject(timer, INFINITE);
  ::CloseHandle(timer);
#endif
}

inline void win_bussy_loop(const Seconds seconds, const double scale) {
  auto st = std::chrono::steady_clock::now();
  win_htimer(seconds * scale);
  while (std::chrono::steady_clock::now() - st <= std_seconds(seconds)) {
    asm volatile ("");
  }
}

// спать 70% времени, остальное ожидать в цикле
inline void win_bussy_loop_70(const Seconds seconds) { return win_bussy_loop(seconds, 0.7); }
// спать 92% времени, остальное ожидать в цикле
inline void win_bussy_loop_92(const Seconds seconds) { return win_bussy_loop(seconds, 0.92); }

#else //LINUX

inline void lin_sleep(const Seconds seconds) {
  std::this_thread::sleep_for(std_seconds(seconds));
}

inline void lin_bussy_loop(const Seconds seconds, const double scale) {
  auto st = std::chrono::steady_clock::now();
  lin_sleep(seconds * scale);
  while (std::chrono::steady_clock::now() - st <= std_seconds(seconds)) {
    asm volatile ("");
  }
}

// спать 70% времени, остальное ожидать в цикле
inline void lin_bussy_loop_70(const Seconds seconds) { return lin_bussy_loop(seconds, 0.7); }
// спать 92% времени, остальное ожидать в цикле
inline void lin_bussy_loop_92(const Seconds seconds) { return lin_bussy_loop(seconds, 0.92); }

#endif

Seconds g_delay_error {0}; // средний оверхед задержки
using Delay_pf = void (*)(const Seconds seconds);

#ifdef WINDOWS
Delay_pf g_delay_pf = &win_bussy_loop_92; // текущая функция для задержки
Str g_timer_name {"win_bussy_loop_92"};
#else // LINUX
Delay_pf g_delay_pf = &lin_bussy_loop_92; // текущая функция для задержки
Str g_timer_name {"lin_bussy_loop_92"};
#endif

void delay_sec(const Seconds seconds) {
  cauto local_delay = seconds - g_delay_error;
  return_if(local_delay <= 0);
  return_if(local_delay >= 3 * 60);
  assert(g_delay_pf);
  g_delay_pf(local_delay);
}

void calibrate_delay(const Seconds target) {
  log_info << "delay timer calibration...";
  log_debug << std::format("целевая задержка {} сек.", target);
  assert(target >= 1.0 / 10'000.0 && target <= 1.0);

  struct Timer_stat {
    Str name {};
    Delay_pf delay_pf {};
    double signed_error {}; //  avg - target     для компенсации
    double abs_error {};    // |avg - target|    для выбора лучшего
  };

  Vector<Timer_stat> statistic {
    Timer_stat {.name="std_delay ............", .delay_pf = &std_delay},
    Timer_stat {.name="std_delay_nop ........", .delay_pf = &std_delay_nop},
    Timer_stat {.name="std_sleep_until ......", .delay_pf = &std_sleep_until},
    #ifdef WINDOWS
    Timer_stat {.name="win_sleep ............", .delay_pf = &win_sleep},
    Timer_stat {.name="win_htimer ...........", .delay_pf = &win_htimer},
    Timer_stat {.name="win_bussy_loop_70 ....", .delay_pf = &win_bussy_loop_70},
    Timer_stat {.name="win_bussy_loop_92 ....", .delay_pf = &win_bussy_loop_92},
    #else // LINUX
    Timer_stat {.name="lin_bussy_loop_70 ....", .delay_pf = &lin_bussy_loop_70},
    Timer_stat {.name="lin_bussy_loop_92 ....", .delay_pf = &lin_bussy_loop_92},
    #endif
  };
  // вычислить отставать ние для каждого таймера и выбрать лучший из них
  constexpr uint TESTS = 10;
  Vector<Seconds> results(TESTS);

  for (rauto stat : statistic) {
    cfor (i, TESTS) {
      cauto st = std::chrono::steady_clock::now();
      stat.delay_pf(target);
      cauto ed = std::chrono::steady_clock::now();
      results[i] = std::chrono::duration_cast<std_seconds>(ed - st).count();
    }

    cauto avg = std::accumulate(results.begin(), results.end(),
      Seconds{0}) / scast<double>(TESTS);
    stat.signed_error = avg - target;
    stat.abs_error = std::abs(stat.signed_error);

    log_debug << "погрешность " << stat.name << " "
              << n2s(stat.signed_error, 14) << " Сек";
  }

  // найти самый точный таймер
  cp<Timer_stat> best_timer = &statistic.at(0);
  for (crauto stat : statistic)
    if (stat.abs_error < best_timer->abs_error)
      best_timer = &stat;

  set_timer(best_timer->name);
  //  set_timer сбрасывает погрешность, так что она обновляется тут:
  g_delay_error = best_timer->signed_error;

  log_debug << std::format("коррекция таймера: {} сек.", n2s(g_delay_error, 14));
}

void set_timer(cr<Str> name) {
  static const std::unordered_map<Str, Delay_pf> table {
    {"std_delay", &std_delay},
    {"std_delay_nop", &std_delay_nop},
    {"std_sleep_until", &std_sleep_until},
    #ifdef WINDOWS
    {"win_sleep", &win_sleep},
    {"win_htimer", &win_htimer},
    {"win_bussy_loop_70", &win_bussy_loop_70},
    {"win_bussy_loop_92", &win_bussy_loop_92},
    #else // LINUX
    {"lin_bussy_loop_70", &lin_bussy_loop_70},
    {"lin_bussy_loop_92", &lin_bussy_loop_92},
    #endif
  };
  try {
    g_delay_pf = table.at(name);
    g_timer_name = name;
  } catch (...) {
    // default:
    g_delay_pf = &std_delay;
    g_timer_name = "std_delay";
  }

  // При ручной смене таймера сбрасываем старую калибровку
  g_delay_error = 0.0;
  log_debug << "selected delay timer: " << g_timer_name;
} // set_timer

Str get_timer() { return g_timer_name; }

Seconds get_cur_time() {
  return std::chrono::duration_cast<std_seconds>(
    std::chrono::steady_clock::now().time_since_epoch()).count();
}
