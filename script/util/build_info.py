from . import helper
import colorama


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
    print(colorama.Fore.RED + f'unknown target system {config.system}' + colorama.Style.RESET_ALL)
    quit()

def prepare(config):
  ''' докидывают дополнительную информацию в конфиг '''
  ver, date, time = helper.get_game_version()
  config.game_ver = ver
  config.commit_date = date
  config.commit_time = time
  config.python_ver = helper.check_python_version()
  if not config.threads or config.threads <= 0:
    config.threads = helper.get_max_threads() + 1
  if not config.bitness or not config.system:
    config.system, config.bitness = helper.get_system_info()
  if not config.target:
    config.target = quess_target(config)
  return config


def is_enable(config, value):
  ''' Enabled Or Disabled '''
  if config.enable and value in config.enable:
    return colorama.Fore.GREEN + "enabled" + colorama.Style.RESET_ALL
  return colorama.Fore.RED + "disabled" + colorama.Style.RESET_ALL

def check_value(value):
  if value:
    return colorama.Fore.YELLOW + str(value) + colorama.Style.RESET_ALL
  return colorama.Fore.LIGHTBLACK_EX + "???" + colorama.Style.RESET_ALL


def print_info(config):
  ''' общая сводка билда '''
  print('\n--------------------------------{ Building info }--------------------------------')
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
  