if __name__ == "__main__":
  quit("не запускай это как главный скрипт")

import sys
import subprocess

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
  return 'unknown'

def game_version():
  ''':return: version, last commit date, last commit time'''
  version = date = time = ""

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
