from .helper import get_game_version, check_python_version, get_max_threads, get_system_info
from colorama import Fore, Style
from os import path as os_path, getenv as os_getenv

def quess_target(config):
  if config.system == 'windows':
    if config.bitness == 'x32':
      return 'win_x32'
    else:
      return 'win_x64'
  elif config.system == 'linux':
    if config.bitness == 'x32':
      return 'lin_x32'
    else:
      return 'lin_x64'
  else:
    quit(Fore.RED + f'unknown target system {config.system}' + Style.RESET_ALL)

def is_enable(config, value):
  ''' Enabled Or Disabled '''
  if config.enable and value in config.enable:
    return Fore.GREEN + "✅ enabled" + Style.RESET_ALL
  return Fore.RED + "❌ disabled" + Style.RESET_ALL

def check_value(value):
  if value:
    return Fore.YELLOW + str(value) + Style.RESET_ALL
  return Fore.LIGHTBLACK_EX + "❔ ???" + Style.RESET_ALL

def need_impl():
  assert False, Fore.BLUE + 'need impl' + Style.RESET_ALL

def prepare_opts(config):
  '''подготавливает опции для GCC/Clang компилятора'''
  config.cxx_flags.extend([
    '-std=c++23',
    '-pipe',
    '-Wall', '-Wfatal-errors', # останавливать компиляцию при первой ошибке
    # '-Wextra', '-pedantic', '-Wno-unused-parameter', # больше ворнингов!
    '-finput-charset=UTF-8', '-fextended-identifiers', # поддержка UTF-8
    '-m32' if config.bitness == 'x32' else '-m64',
  ])
  config.ld_flags.append('-pipe'),
  config.defines.extend(['-DWINDOWS' if config.system == 'windows' else '-DLINUX'])
  if config.system == 'linux':
    config.cxx_flags.extend(['-fdiagnostics-color=always'])
  match config.target:
    case 'win_x64_debug' | 'win_x32_debig' | 'lin_x64_debug':
      config.cxx_flags.extend(['-O0', '-g0'])
      config.defines.extend(['-DDEBUG'])
    case 'win_xp': quit(Fore.RED + 'now Windows XP is not supported' + Style.RESET_ALL) # TODO
    case 'win_atom':
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=atom', '-mtune=atom'])
      config.ld_flags.extend(['-mwindows', '-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'win_core2':
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=core2', '-mtune=core2'])
      config.ld_flags.extend(['-mwindows', '-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'win_x64':
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=x86-64', '-mtune=generic'])
      config.ld_flags.extend(['-mwindows', '-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'lin_x64':
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=x86-64', '-mtune=generic'])
      config.ld_flags.extend(['-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'win_x64_v4':  
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=x86-64-v4', '-mtune=generic'])
      config.ld_flags.extend(['-mwindows', '-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'lin_x64_v4':  
      config.cxx_flags.extend(['-Ofast', '-flto=auto', '-march=x86-64-v4', '-mtune=generic'])
      config.ld_flags.extend(['-flto=auto'])
      config.defines.extend(['-DNDEBUG', '-DRELEASE'])
    case 'lin_x32': need_impl() # TODO
    case _: quit(Fore.RED + f'unknown target {config.target}' + Style.RESET_ALL)
  return config

def prepare(config):
  ''' докидывают дополнительную информацию в конфиг '''
  ver, date, time = get_game_version()
  config.game_ver = ver
  config.commit_date = date
  config.commit_time = time
  config.python_ver = check_python_version()
  if not config.threads or config.threads <= 0:
    config.threads = get_max_threads() + 1
  if not config.bitness or not config.system:
    config.system, config.bitness = get_system_info()
  if not config.target:
    config.target = quess_target(config)
  if config.system == 'windows' and config.enable and 'asan' in config.enable:
    quit(Fore.RED + f'ASAN not allowed in Windows' + Style.RESET_ALL)
  if not config.cxx:
    os_cxx = os_getenv('CXX')
    if os_cxx:
      config.cxx = os_cxx
    else:
      config.cxx = 'g++'
  if not config.cc:
    os_cc = os_getenv('CC')
    if os_cc:
      config.cc = os_cc
    else:
      config.cc = 'gcc'
  config = prepare_opts(config)
  return config

def print_info(config):
  ''' общая сводка билда '''
  print('\n--------------------------------{ Building info }--------------------------------')
  if config.info:
    print(f'argparse args: {check_value(config)}')
    print('')

  print(f"Python version: {check_value(config.python_ver)}")
  print(f"Game version: {check_value(config.game_ver)}")
  print(f"Last commit: {check_value(config.commit_date)}")
  
  print('')
  print(f'C++ compiler: {check_value(config.cxx)}')
  print(f'C compiler: {check_value(config.cc)}')
  print(f'Building threads: {check_value(config.threads)}')
  print(f'Target: {check_value(config.target)}')
  print(f'Current system: {check_value(config.system)} {check_value(config.bitness)}')
  print(f'Static linking: {is_enable(config, "static")}')
  print(f'ASAN: {is_enable(config, "asan")}')
  print(f'OMP: {is_enable(config, "omp")}')

  print('')
  print(f'Linker flags: {check_value(config.ld_flags)}')
  print(f'C++ flags: {check_value(config.cxx_flags)}')
  print(f'C++ Defines: {check_value(config.defines)}')

  print('')
  print(f'Replays: {is_enable(config, "replay")}')
  print(f'LAN game: {is_enable(config, "netplay")}')
  print(f'Sounds: {is_enable(config, "sound")}')
  print(f'Config I/O: {is_enable(config, "config")}')
  print(f'Data.zip I/O: {is_enable(config, "data")}')
  
def dir_exists(name):
  if os_path.isdir(name):
    print(f'directory \"{name}\" founded')
  else:
    quit(Fore.RED + f'directory \"{name} not founded' + Style.RESET_ALL)

def file_exists(name):
  if os_path.isfile(name):
    print(f'file \"{name}\" founded')
  else:
    quit(Fore.RED + f'file \"{name} not founded' + Style.RESET_ALL)

def env_test():
  print('\n--------------------------------{ Check folders }--------------------------------')
  dir_exists('./.tmp')
  dir_exists('./build')
  dir_exists('./src')
  dir_exists('./thirdparty')
  dir_exists('./script')
  dir_exists('./data')
  dir_exists('./test')
  file_exists('./SConstruct')
  file_exists('./test/cxx/main.cpp')
