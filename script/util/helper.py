from sys import version_info as sys_version_info
from platform import system as platform_system, architecture as platform_architecture
from os import chdir, path as os_path, remove as os_remove, makedirs, linesep as os_linesep
from os import cpu_count as os_cpu_count
from time import time
from subprocess import PIPE as subproc_pipe, run as subproc_run, check_output as subproc_check_output
from glob import glob
from shutil import rmtree, copyfile

def set_work_dir(path):
  "cd"
  print (f'set working dir: \"{path}\"')
  chdir(path)

def rem_if_exist(fname):
  "удалить файл, если он есть"
  if os_path.exists(fname):
    print (f'remove {fname}')
    os_remove(fname)

def rem_dir(dname, ignore_errors=True):
  "удалить папку"
  rmtree(dname, ignore_errors=ignore_errors, onerror=None)
  assert os_path.exists (dname) == False, f"dir \"{dname}\" is not deleted!"

def rem_all(fname_mask):
  "удалить по маске"
  list = glob (fname_mask, recursive=True)
  for fname in list:
    rem_if_exist (fname)

def exec_cmd(cmd, without_print=False):
  '''выполнить команду
  
  :return: вывод команды stdout, stderr'''
  cmd_tm_st = time()
  cmd = os_path.normpath (cmd) 
  print (cmd)
  if without_print:
    result = subproc_run(cmd.split(), check=True, universal_newlines=True,
      stdout=subproc_pipe, stderr=subproc_pipe)
  else:
    result = subproc_run(cmd.split(), check=True)
  cmd_tm_ed = time()
  print (f'e.t: { round(cmd_tm_ed - cmd_tm_st, 1) }s')
  print()
  return result.stdout, result.stderr

def get_game_version():
  ''':return: version, last commit date, last commit time'''
  version = ""
  date = ""
  time = ""

  # получить версии
  try:
    cmd_ver = "git describe --tags --abbrev=0"
    cmd_date = "git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y"
    cmd_time = "git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M"
    # выполнить команды и сохранить их вывод из консоли
    version = subproc_check_output(cmd_ver.split()).decode().strip()
    date = subproc_check_output(cmd_date.split()).decode().strip()
    time = subproc_check_output(cmd_time.split()).decode().strip()
  except:
    print("[!] Error when getting game version")

  return version, date, time

def write_game_version():
  "записать версию игры в version.cpp"

  version, date, time = get_game_version()
  print("game version: " + version if version else "unknown")
  print("last commit date: " + date if date else "unknown")
  print("last commit time: " + time if time else "unknown")

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
    makedirs("build/info/", exist_ok=True)
    copyfile('LICENSE.txt', 'build/info/LICENSE.txt')
    copyfile('NOTICE.txt',  'build/info/NOTICE.txt')
  except:
    print("error copying license file")

def prepare_strs(strs: list[str]):
  return ' '.join(filter(None, strs))

def save_version(build_dir, used_libs, hpw_config, cxx, cc):
  "создаёт файл build_dir/info/version info с инфой о компиляции"
  try:
    makedirs("build/info/", exist_ok=True)

    with open(file=f'{build_dir}info/version info.txt', mode='w', encoding="utf-8", newline=os_linesep) as file:
      game_ver, game_date, game_time = get_game_version()
      file.writelines([
        "H.P.W build info:\n",
        f'  ASAN checks: {hpw_config.enable_asan}\n',
        f'  Game ver: {game_ver if game_ver else "unknown"}\n',
        f'    last commit date: {game_date if game_date else "unknown"}\n',
        f'    last commit time: {game_time if game_time else "unknown"}\n',

        f'\nUtils:\n',
        f'  Python ver: {check_python_version()}\n',
        f'  SCons build script: \"{hpw_config.build_script}\"\n',

        f'\nTarget:\n',
        f'  OS: {hpw_config.system.name + ' ' + hpw_config.bitness.name}\n',
        f'  Host render-system: {hpw_config.host.name}\n',
        f'  Netplay: {hpw_config.use_netplay}\n',

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
    print(f"error generating \"info/version info.txt\"\n{inst}")

def get_max_threads():
  '''позволяет узнать сколько доступно потоков процессора'''
  try:
    return os_cpu_count()
  except:
    print('ошибка при получении числа потоков процессора; Будет использоваться 1 по умолчанию')
    return 1

def check_python_version():
  '''проверка нужной версии интерпритатора'''
  try:
    ver = sys_version_info
    if ver < (3,12,0):
      print('WARNING: требуется версия python не ниже 3.12.0')
    else:
      str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
      return str_ver
  except:
    print(f'error while getting version of python')
  return 'unknown'

def get_system_info():
  ''':return: system name, bitness'''
  name = platform_system() # 'Windows', 'Linux', 'Darwin' (macOS)
  bitness = 'x32' if platform_architecture()[0] == '32bit' else 'x64'
  return name.lower(), bitness