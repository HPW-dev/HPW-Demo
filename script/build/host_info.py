if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import sys
import subprocess
import script.build.util as util
import platform

def python_version():
  '''проверка нужной версии Python'''
  try:
    ver = sys.version_info
    if ver < (3,12,0):
      print('WARNING: требуется версия python не ниже 3.12.0')
    else:
      str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
      return str_ver
  except:
    print(f'error while getting version of python')
  return None

def scons_version():
  '''узнать версию Scons'''
  try:
    out, _ = util.exec_cmd("scons --version", True)
    return out
  except:
    print(f'error while getting SCons version')
  return None

def game_version():
  ''':return: version, last commit date, last commit time'''
  version = date = time = None

  # получить версии
  try:
    cmd_ver = "git describe --tags --abbrev=0"
    cmd_date = "git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y"
    cmd_time = "git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M"
    # выполнить команды и сохранить их вывод из консоли
    version = subprocess.check_output(cmd_ver.split() ).decode().strip()
    date    = subprocess.check_output(cmd_date.split()).decode().strip()
    time    = subprocess.check_output(cmd_time.split()).decode().strip()
  except:
    print("[!] Error when getting game version")

  return version, date, time

def system_name():
  ''':return: Windows | Linux | Darwin (macOS)'''
  return platform.system()

def system_bitness():
  ''':return: x32 | x64'''
  return 'x32' if platform.architecture()[0] == '32bit' else 'x64'

def prepare():
  info = {}
  info["bitness"] = system_bitness()
  info["system"] = system_name()
  ver, commit_date, commit_time = game_version()
  info["game_ver"] = ver
  info["commit_date"] = commit_date
  info["commit_time"] = commit_time
  info["python_ver"] = python_version()
  info["scons_ver"] = scons_version()
  return info
