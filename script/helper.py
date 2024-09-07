import os
import time
import subprocess
import glob
import shutil

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
  with open(file='src/game/util/version.cpp', mode='w', newline='\n') as file:
    file.write (
      '#include "version.hpp"\n'
      '\n'
      'const char* get_game_version() { return "' + version + '"; }\n'
      'const char* get_game_creation_date() { return "' + date + '"; }\n'
      'const char* get_game_creation_time() { return "' + time + '"; }\n'
    )

def copy_license():
  "копирует файл LICENSE.txt в нужную папку"
  try:
    #shutil.copyfile('source.txt', 'destination.txt')
    pass
  except:
    print("error copying license file")
