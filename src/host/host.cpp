#include <iostream>
#include <ctime>
#include "host.hpp"
#include "host-util.hpp"
#include "command.hpp"
#include "host-resize.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/tasks.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/logo.hpp"
#include "game/util/game-util.hpp"
#include "util/log.hpp"
#include "util/path.hpp"
#include "util/pparser.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/color-table.hpp"

// если перенести это вверх, то всё взорвётся >_<
#ifdef WINDOWS
#include <windows.h>
#endif

// отложенный выбор фуллскрина
class Task_fullscreen final: public Task {
  bool _mode {};

public:
  inline explicit Task_fullscreen(bool mode): _mode {mode} {}

  inline void update(const Delta_time dt) override final {
    assert(hpw::set_fullscreen);
    hpw::set_fullscreen(_mode);
    this->deactivate();
  }
};

struct Host::Impl final {
  nocopy(Impl);
  Host& _master;
  std::optional<uint32_t> custom_seed {};

  explicit inline Impl(Host& master): _master {master} {}

  inline void parse_args(int argc, char** argv) {
    Pparser ret( Pparser::v_param_t {
      {{"-s", "--seed"}, "set random seed", [this](cr<Str> val){ custom_seed = s2n<uint32_t>(val); }},
      {{"-w", "--windowed"}, "enable windowed mode", [this](cr<Str> val){ hpw::global_task_mgr.add(new_shared<Task_fullscreen>(false)); }},
      {{"-f", "--fullscreen"}, "enable fullscreen mode", [this](cr<Str> val){ hpw::global_task_mgr.add(new_shared<Task_fullscreen>(true)); }},
      {{"-h", "--help", "--info"}, "print this help and close the game", [&](cr<Str> val){
        ret.print_info();
        std::exit(EXIT_SUCCESS);
      }},
    } );
    ret.skip_empty = true;
    ret(argc, argv);
  }
}; // Impl

Host::Host(int argc, char** argv)
: m_argc(argc)
, m_argv(argv)
, _impl (new_unique<Impl>(*this))
{
  // парс аргументов:
  hpw::argc = m_argc;
  hpw::argv = m_argv;
  _impl->parse_args(hpw::argc, hpw::argv);

  init_app_mutex();

#ifdef WINDOWS
  // переключить консоль винды в UTF8
  SetConsoleOutputCP(CP_UTF8);
#endif

  // настроить генерацию рандома
  uint32_t seed;
  if (_impl->custom_seed.has_value())
    seed = _impl->custom_seed.value();
  else
    seed = time({});
  set_rnd_seed(seed);
  detailed_log("Сид рандома: " << seed << '\n');

  std::cout << '\n';
  std::cout << get_random_logo();
  std::cout << '\n';
  std::cout << "Запуск H.P.W\n";
  std::cout.flush();

  callbacks_init();

  // узнать в какойо папке игра запущена
  hpw::cur_dir = launch_dir_from_argv0(argv[0]);

  detailed_log("Директория запуска игры: \"" << hpw::cur_dir << "\"\n");
  load_config();
} // c-tor

Host::~Host() {
  save_config();
  free_app_mutex();
  std::cout << "Корректное завершение H.P.W" << std::endl;
}

void Host::exit() {
  hpw_log("вызов программного выхода из игры...\n");
  m_is_ran = false;
}
 
void Host::callbacks_init() {
  hpw::soft_exit = [this] { this->exit(); };
  hpw::make_screenshot = [this] {
    if (graphic::canvas)
      save_screenshot(*graphic::canvas);
    else
      hpw_log("не удалось сохранить скриншот, так как graphic::canvas ещё не инициализирован\n");
  };
}

void Host::init_app_mutex() {
#ifdef WINDOWS
  m_app_mutex = CreateMutex(NULL, TRUE, "HPW: Double window mutex");
  if (ERROR_ALREADY_EXISTS == GetLastError()) {
    hpw::multiple_apps = true;
    CloseHandle(m_app_mutex);
  }
#else
  #pragma message("need impl for other_game_started() on Linux")
#endif
}

void Host::free_app_mutex() {
#ifdef WINDOWS
  ReleaseMutex(m_app_mutex);
  CloseHandle(m_app_mutex);
#else
  #pragma message("need impl for free_app_mutex() on Linux")
#endif
}

void Host::update(const Delta_time dt) {
  hpw::global_task_mgr.update(dt);
}
