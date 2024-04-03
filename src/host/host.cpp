#include <iostream>
#include <ctime>
#include "host.hpp"
#include "host-util.hpp"
#include "command.hpp"
#include "resize.hpp"
#include "game/core/common.hpp"
#include "game/util/sync.hpp"
#include "game/util/config.hpp"
#include "game/util/logo.hpp"
#include "util/log.hpp"
#include "util/path.hpp"
#include "util/math/random.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/color-table.hpp"

// если перенести это вверх, то всё взорвётся >_<
#ifdef WINDOWS
#include <windows.h>
#endif

Host::Host(int _argc, char** _argv)
: argc(_argc)
, argv(_argv)
{
  hpw::argc = _argc;
  hpw::argv = _argv;

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
  std::cout << "Корректное завершение H.P.W" << std::endl;
}

void Host::exit() {
  detailed_log("Host.exit\n");
  is_run = false;
}
 
void Host::callbacks_init() {
  hpw::soft_exit = [this] { this->exit(); };
  hpw::set_fullscreen = [this](bool enable) { this->_set_fullscreen(enable); };
  hpw::set_double_buffering = [this](bool enable) { this->_set_double_buffering(enable); };
  hpw::set_gamma = [this](const double val) { this->set_gamma(val); };
  hpw::make_screenshot = [this] { this->save_screenshot(); };
  hpw::set_resize_mode = [this](Resize_mode mode) { this->_set_resize_mode(mode); };
  hpw::set_mouse_cursour_mode = [this](bool enable) { this->_set_mouse_cursour_mode(enable); };
}
