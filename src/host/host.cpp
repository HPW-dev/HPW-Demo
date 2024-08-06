#include <iostream>
#include <ctime>
#include "host.hpp"
#include "host-util.hpp"
#include "command.hpp"
#include "host-resize.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/logo.hpp"
#include "game/util/game-util.hpp"
#include "util/log.hpp"
#include "util/path.hpp"
#include "util/math/random.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/color-table.hpp"

// если перенести это вверх, то всё взорвётся >_<
#ifdef WINDOWS
#include <windows.h>
#endif

Host::Host(int argc, char** argv)
: m_argc(argc)
, m_argv(argv)
{
  hpw::argc = m_argc;
  hpw::argv = m_argv;
  init_app_mutex();

#ifdef WINDOWS
  // переключить консоль винды в UTF8
  SetConsoleOutputCP(CP_UTF8);
#endif

  // настроить генерацию рандома
  uint32_t seed = time({});
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
  detailed_log("Host.exit\n");
  m_is_ran = false;
}
 
void Host::callbacks_init() {
  hpw::soft_exit = [this] { this->exit(); };
  hpw::set_fullscreen = [this](bool enable) { this->_set_fullscreen(enable); };
  hpw::set_double_buffering = [this](bool enable) { this->_set_double_buffering(enable); };
  hpw::set_resize_mode = [this](Resize_mode mode) { this->_set_resize_mode(mode); };
  hpw::set_mouse_cursour_mode = [this](bool enable) { this->_set_mouse_cursour_mode(enable); };

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
