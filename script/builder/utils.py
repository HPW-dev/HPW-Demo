if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import script.builder.io.platform as platform
import script.builder.distr.compiler as compiler
import script.builder.args as args
import script.builder.ui as ui
import re
import os
import subprocess
import datetime
import script.builder.io.fs as fs
from script.builder.ui import *
from script.builder.distr.compiler import *

def concat_strlist(strs: list[str]):
  return ' '.join(filter(None, strs))

def extend_vars():
  'добавляет дополнительные параметры в окружение'
  ret = {}
  ret['build_dir'] = './build/'
  ret['data_dir'] = './data/'
  ret['data_archive_path'] = f'{ret['build_dir']}data.zip'
  ret['bin_dir'] = f'{ret['build_dir']}bin/'
  ret['info_dir'] = f'{ret['build_dir']}info/'
  ret['ld_flags']  = []
  ret['cxx_flags'] = []
  ret['defines']   = []
  ret['generation_date'] = datetime.datetime.now().strftime("%d.%m.%Y")
  ret['generation_time'] = datetime.datetime.now().strftime("%H:%M:%S")

  if not fs.check_dir(ret['build_dir']): quit(txt_red(f'директория {ret['build_dir']} не найдена'))
  if not fs.check_dir(ret['bin_dir']): quit(txt_red(f'директория {ret['bin_dir']} не найдена'))
  return ret

def list2s(params: list[str], add_sym="") -> str:
  ":return: params объединённые в одну строку и начинающиеся на add_sym"
  ret = ""
  for x in params:
    ret += f'{add_sym}{x} '
  return ret.strip() # в начале и в конце убратьпробел

def extract_includes(file_path: str, max_lines=40):
  "Извлекает список файлов из директив #include в начале C++ файла"
  includes = []
  pattern = re.compile(r'^\s*#include\s*["](.*?)["]')
  try:
    with open(file_path, 'r', encoding='utf-8') as file:
      for line_number, line in enumerate(file):
        if line_number >= max_lines:
          break
        match = pattern.match(line)
        if match:
          included_file = match.group(1)
          includes.append(included_file)
  except IOError as e:
    print(f"Ошибка чтения файла {file_path}: {e}")
  
  # пути к файлам должны начинаться с рута:
  includes = list( map(
    lambda x: x if os.path.exists(x) else f'{os.path.dirname(file_path)}/{x}',
    includes
  ) )
  return includes

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
    print(txt_red("[!] Error when getting game version"))

  return version, date, time

def init_env():
  env = os.environ.copy()             # в системе уже могут быть свои переменные
  env.update(args.parse())            # получить параметры с аргументов запуска
  env.update(platform.sys_info(env))  # узнать параметры у системы

  # переопределить настройки компилятора:
  if 'cxx' in env and env['cxx'] != None: env['CXX'] = env['cxx']
  if 'cc'  in env and env['cc']  != None: env['CC']  = env['cc']
  if 'ld'  in env and env['ld']  != None: env['LD']  = env['ld']
  env["compiler_ver"] = compiler.compiler_version(env)

  # если указано 0 потоков, то взять оптимальное число
  if env['threads'] <= 0: env['threads'] = platform.max_threads() + 1
  return env

def clean_all():
  'удаляет объектники, экзешники, скрины и логи'
  print(ui.txt_green('очистка файлов сборки...'))
  fs.rem_all('**/delme.html')
  fs.rem_all('**/*.exe')
  fs.rem_all('**/*.elf')
  fs.rem_all('**/*.elf32')
  fs.rem_all('**/*.elf64')
  fs.rem_all('**/vgcore.*')
  fs.rem_all('**/cpplint.txt')
  fs.rem_all('**/gmon.svg')
  fs.rem_all('**/*.obj')
  fs.rem_all('**/*.o')
  fs.rem_all('**/*.os')
  fs.rem_all('**/*.out')
  fs.rem_all('**/*.gch')
  fs.rem_all('**/log.txt')
  fs.rem_all('**/*.gcda')
  fs.rem_all('**/*.gcno')
  fs.rem_all('**/*.hpw_replay')
  fs.rem_all('**/imgui.ini')
  fs.rem_all('**/cpplint.txt')
  fs.rem_all('**/perf.data.old')
  fs.rem_all('**/perf.data')
  fs.rem_dir('__pycache__')
  fs.rem_dir('script/__pycache__')
  fs.rem_dir('pgo', True)
  fs.rem_if_exist('build/bin/HPW.exe')
  fs.rem_if_exist('build/bin/HPW')
  fs.rem_if_exist('build/test.yml')
  fs.rem_if_exist('.sconsign.dblite')
  fs.rem_all('.tmp/*.txt')
  fs.rem_all('.tmp/*.pickle')
  fs.rem_dir('build/screenshots/', True)
  #fs.rem_all('build/replays/*')
  fs.rem_dir('build/replays/', True)
  fs.rem_dir('build/plugin/', True)
  fs.rem_all('build/debug.txt')
  fs.rem_all('build/data.zip')
  fs.rem_dir('build/config/', True)
  fs.rem_dir('build/info/', True)
