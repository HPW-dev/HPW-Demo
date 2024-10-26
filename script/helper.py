import sys
import os
import time
import subprocess
import glob
import shutil
from enum import Enum

Bitness = Enum('Bitness', ['x32', 'x64'])
System = Enum('System', ['windows', 'linux'])
Compiler = Enum('Compiler', ['gcc', 'clang', 'msvc'])
Opt_level = Enum('Opt_level', ['fast', 'debug', 'optimized_debug', 'stable',
  'x86_64_v1', 'x86_64_v4', 'ecomem', 'core2', 'atom', 'i386_stable', 'i386'])
Host = Enum('Host', ['glfw3', 'sdl2', 'asci', 'none'])
Log_mode = Enum('Log_mode', ['detailed', 'debug', 'release'])

class Hpw_config:
  '''хранит настройки билда'''
  # управляющие переменные:
  bitness = Bitness.x64
  system = System.windows
  compiler = Compiler.gcc
  opt_level = Opt_level.debug
  host = Host.glfw3
  log_mode = Log_mode.debug
  enable_omp = True
  enable_asan = False
  static_link = False
  build_script = ''
  custom_cxx: str
  custom_cc: str

  # опции билда C++:
  cxx_defines = []
  cxx_ldflags = []
  cxx_flags = []

def set_work_dir(path):
  "cd"
  print (f'set working dir: \"{path}\"')
  os.chdir(path)

def rem_if_exist(fname):
  "удалить файл, если он есть"
  if os.path.exists (fname):
    print (f'remove {fname}')
    os.remove (fname)

def rem_dir(dname, ignore_errors=True):
  "удалить папку"
  shutil.rmtree (dname, ignore_errors=ignore_errors, onerror=None)
  assert os.path.exists (dname) == False, f"dir \"{dname}\" is not deleted!"

def rem_all(fname_mask):
  "удалить по маске"
  list = glob.glob (fname_mask, recursive=True)
  for fname in list:
    rem_if_exist (fname)

def exec_cmd(cmd):
  "выполнить команду"
  cmd_tm_st = time.time()
  cmd = os.path.normpath (cmd) 
  print (cmd)
  subprocess.run (cmd.split(), check=True)
  cmd_tm_ed = time.time()
  print (f'e.t: { round(cmd_tm_ed - cmd_tm_st, 1) }s')
  print()

def get_game_version():
  ''':return: version, last commit date, last commit time'''
  version = "vMj.Mn.Mc"
  date = "DD.MM.YYYY"
  time = "HH:MM"

  # получить версии
  try:
    cmd_ver = "git describe --tags --abbrev=0"
    cmd_date = "git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y"
    cmd_time = "git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M"
    # выполнить команды и сохранить их вывод из консоли
    version = subprocess.check_output(cmd_ver.split()).decode().strip()
    date = subprocess.check_output(cmd_date.split()).decode().strip()
    time = subprocess.check_output(cmd_time.split()).decode().strip()
  except:
    print("[!] Error when getting game version")

  return version, date, time

def write_game_version():
  "записать версию игры в version.cpp"

  version, date, time = get_game_version()
  print("game version: " + version)
  print("last commit date: " + date)
  print("last commit time: " + time)

  # сгенерировать C++ файл
  with open(file='src/game/util/version.cpp', mode='w', newline='\n', encoding="utf-8") as file:
    file.write (
      '#include "version.hpp"\n'
      '\n'
      'const char* get_game_version() { return "' + version + '"; }\n'
      'const char* get_game_creation_date() { return "' + date + '"; }\n'
      'const char* get_game_creation_time() { return "' + time + '"; }\n'
    )

def copy_license():
  "копирует файл LICENSE.txt и NOTICE.txt в нужную папку"
  try:
    shutil.copyfile('LICENSE.txt', 'build/LICENSE.txt')
    shutil.copyfile('NOTICE.txt',  'build/NOTICE.txt')
  except:
    print("error copying license file")

def prepare_strs(strs: list[str]):
  return ' '.join(filter(None, strs))

def save_version(build_dir, used_libs, hpw_config, cxx, cc):
  "создаёт файл build_dir/BUILD.txt с инфой о компиляции"
  try:
    with open(file=f'{build_dir}BUILD.txt', mode='w', encoding="utf-8", newline=os.linesep) as file:
      game_ver, game_date, game_time = get_game_version()
      file.writelines([
        "H.P.W build info:\n",
        f'  ASAN checks: {hpw_config.enable_asan}\n',
        f'  Log mode: {hpw_config.log_mode.name}\n',
        f'  Game ver: {game_ver}\n',
        f'    last commit date: {game_date}\n',
        f'    last commit time: {game_time}\n',

        f'\nUtils:\n',
        f'  Python ver: {check_python_version()}\n',
        f'  SCons build script: \"{hpw_config.build_script}\"\n',

        f'\nTarget:\n',
        f'  OS: {hpw_config.system.name + ' ' + hpw_config.bitness.name}\n',
        f'  Host render-system: {hpw_config.host.name}\n',

        f'\nCompilation info:\n',
        f'  Compiler: {hpw_config.compiler.name}\n',
        f'  CXX: {cxx if cxx else 'default'}\n',
        f'  CC: {cc if cc else 'default'}\n',
        f'  Optimization level: {hpw_config.opt_level.name}\n',
        f'  defines: {prepare_strs(hpw_config.cxx_defines)}\n',
        f'  flags: {prepare_strs(hpw_config.cxx_flags)}\n',

        f'\nLinking info:\n',
        f'  Static link: {hpw_config.static_link}\n',
        f'  LD flags: {prepare_strs(hpw_config.cxx_ldflags)}\n',
        f'  Used LIB\'s: {prepare_strs(used_libs)}\n',
        f'  OpenMP: {hpw_config.enable_omp}\n',
      ])
  except Exception as inst:
    print(f"error generating build/BUILD.txt\n{inst}")

def get_max_threads():
  '''позволяет узнать сколько доступно потоков процессора'''
  try:
    return os.cpu_count()
  except:
    print('ошибка при получении числа потоков процессора; Будет использоваться 1 по умолчанию')
    return 1

def check_python_version():
  '''проверка нужной версии интерпритатора'''
  try:
    ver = sys.version_info
    if ver < (3,12,0):
      print('WARNING: требуется версия python не ниже 3.12.0')
    else:
      str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
      return str_ver
  except:
    print(f'error while getting version of python')
  return 'unknown'