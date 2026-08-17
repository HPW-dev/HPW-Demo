from structs.context import *
from utils.exec import exec_cmd
from utils.ui import *
from sys import stderr

def game_ver(ctx: Context):
  '''
  Узнать версию игры по комитам в гите
  
  result:
    (ver, date, time)
  '''
  version, date, time = '', '', ''
  
  try:
    cmd_ver = 'git describe --tags --abbrev=0'
    cmd_date = 'git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y'
    cmd_time = 'git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M'
    # выполнить команды и сохранить их вывод из консоли
    version, _, _ = exec_cmd(cmd_ver.split())
    date, _, _ = exec_cmd(cmd_date.split())
    time, _, _ = exec_cmd(cmd_time.split())
  except Exception as ex:
    print(to_red(f'Ошибка при получении версии игры:\n{ex}', file=stderr))
  
  return version.strip(), date.strip(), time.strip()

def generate_game_ver_file(ctx: Context):
  '''Сгенерировать код для получения версии игры в самой игре'''
  pass
