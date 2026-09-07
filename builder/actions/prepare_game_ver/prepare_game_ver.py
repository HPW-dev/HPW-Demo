from structs.context import *
from utils.exec import exec_cmd
from utils.ui import *
from utils.fs import path_abs, exists
from sys import stderr

def game_version(ctx: Context):
  '''
  Узнать версию игры по комитам в гите
  
  :result:
    (ver, date, time)
  '''
  
  try:
    cmd_ver = 'git describe --tags --abbrev=0'
    cmd_date = 'git --no-pager log -1 --pretty=format:%cd --date=format:%d.%m.%Y'
    cmd_time = 'git --no-pager log -1 --pretty=format:%cd --date=format:%H:%M'
    # выполнить команды и сохранить их вывод из консоли
    version, _, _ = exec_cmd(cmd_ver.split())
    date, _, _ = exec_cmd(cmd_date.split())
    time, _, _ = exec_cmd(cmd_time.split())
    if version: version = version.strip()
    if date: date = date.strip()
    if time: time = time.strip()
    return version, date, time
  
  except Exception:
    print(to_red('Ошибка при получении версии игры'), file=stderr)
    return '> v0.270.0.0', '> 17.08.2026', '> 15:11'

def generate_game_version_file(ctx: Context):
  '''Сгенерировать код для получения версии игры в самой игре'''
  version, date, time = game_version(ctx)

  fname = path_abs(f'{ctx.src_dir}/game/util/version.cpp') 
  print(to_gray(f'генерация файла версии игры "{fname}"...'))

  with open(fname, 'w', newline='\n', encoding="utf-8") as f:
    f.write (
      '#include "version.hpp"\n'
      '\n'
      f'const char app_version[] __attribute__((section(".comment"))) = "FileVersion: {version.replace('v','')}";'
      '// @(#) - версия для Linux прог what и ident\n'
      f'static const char rc_version[] = "@(#) H.P.W game {version} (c) 2020-2026 HPW-Dev";\n'
      'const char* get_game_version() { return "' + version + '"; }\n'
      'const char* get_game_creation_date() { return "' + date + '"; }\n'
      'const char* get_game_creation_time() { return "' + time + '"; }\n'
    )

  if not exists(fname):
    raise FileExistsError(f'Не удалось создать файл с версией игры "{fname}"')
