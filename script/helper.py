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
  'x86_64_v1', 'x86_64_v4', 'ecomem', 'core2'])
Host = Enum('Host', ['glfw3', 'sdl2', 'asci', 'none'])
Log_mode = Enum('Log_mode', ['detailed', 'debug', 'release'])

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

def write_game_version():
  "записать версию игры в version.cpp"
  version = "v?.?.?"
  date = "??.??.??"
  time = "??:??"

  # получить версии
  try:
    cmd_ver = "git describe --tags --abbrev=0"
    cmd_date = "git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y"
    cmd_time = "git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M"
    # выполнить команды и сохранить их вывод из консоли
    version = subprocess.check_output(cmd_ver.split()).decode().strip()
    date = subprocess.check_output(cmd_date.split()).decode().strip()
    time = subprocess.check_output(cmd_time.split()).decode().strip()
    print("game version: " + version)
    print("last commit date: " + date)
    print("last commit time: " + time)
  except:
    print("[!] Error when getting game version")
  
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

def save_version(build_dir, used_libs, compiler, defines, cpp_flags, ld_flags, system, bitness, host):
  "создаёт файл build_dir/BUILD.txt с инфой о компиляции"
  try:
    with open(file=f'{build_dir}BUILD.txt', mode='w', encoding="utf-8", newline=os.linesep) as file:
      file.writelines([
        "### H.P.W BUILD INFO\n",
        f"* Compiler: {compiler}\n",
        f"* OS: {system} {bitness}\n",
        f"* Host-render system: {host}\n",
        f"* C/CXX defines: {prepare_strs(defines)}\n",
        f"* C/CXX flags: {prepare_strs(cpp_flags)}\n",
        f"* LD flags: {prepare_strs(ld_flags)}\n",
        f"* Used LIB's: {prepare_strs(used_libs)}\n",
      ])
  except:
    print("error generating build/BUILD.txt")

def get_max_threads():
  '''позволяет узнать сколько доступно потоков процессора'''
  try:
    return os.cpu_count()
  except:
    print('ошибка при получении числа потоков процессора; Будет использоваться 1 по умолчанию')
    return 1
