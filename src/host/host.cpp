#include <cassert>
#include <format>
#include <ctime>
#include "host.hpp"
#include "host-util.hpp"
#include "host-resize.hpp"
#include "command.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/tasks.hpp"
#include "game/core/user.hpp"
#include "game/core/debug.hpp"
#include "game/core/graphic.hpp"
#include "game/core/core.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/logo.hpp"
#include "game/util/version.hpp"
#include "game/util/keybits.hpp"
#include "game/util/screenshot.hpp"
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
    this->kill();
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
      {{"-t", "--title"}, "set window title", [this](cr<Str> val){ hpw::default_tile = val; }},
      {{"-w", "--windowed"}, "enable windowed mode", [this](cr<Str> val){ hpw::global_task_mgr.add(new_shared<Task_fullscreen>(false)); }},
      {{"-f", "--fullscreen"}, "enable fullscreen mode", [this](cr<Str> val){ hpw::global_task_mgr.add(new_shared<Task_fullscreen>(true)); }},
      {{"-h", "--help", "--info"}, "print this help and exit", [&](cr<Str> val){
        ret.print_info();
        std::exit(EXIT_SUCCESS);
      }},
      {{"-v", "--version"}, "print game version and exit", [this](cr<Str> val){
        cauto ver = get_game_version();
        cauto creation_date = get_game_creation_date();
        cauto creation_time = get_game_creation_time();
        hpw_log(std::format("game version: {} ({} {})\n", ver, creation_date, creation_time));
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
  hpw_log(std::format("Сид рандома: {}\n", seed), Log_stream::debug);

  hpw_log('\n' + get_random_logo() + '\n');
  callbacks_init();

  // узнать в какойо папке игра запущена
  hpw::cur_dir = launch_dir_from_argv0(argv);

  #ifndef DISABLE_CONFIG
    load_config();
  #endif
  hpw_log("Директория запуска игры: \"" + hpw::cur_dir + "\"\n", Log_stream::debug);
} // c-tor

Host::~Host() {
  #ifndef DISABLE_CONFIG
    save_config();
  #endif
  free_app_mutex();
  hpw_log("Корректное завершение H.P.W\n");
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
  ++hpw::global_ticks;
  process_input();
}

void Host::process_input() {
  // обработка специальных кнопок
  if (is_pressed_once(hpw::keycode::fulscrn)) {
    assert(hpw::set_fullscreen);
    hpw::set_fullscreen( !graphic::fullscreen );
  }

  if (is_pressed_once(hpw::keycode::screenshot)) {
    assert(hpw::make_screenshot);
    hpw::make_screenshot();
  }

  if (is_pressed_once(hpw::keycode::fps))
    graphic::show_fps = !graphic::show_fps;
}
