if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

from script.build.pretty_txt import *
import json
import os

def print_info(env):
  '''показывает сводку о параметрах билда'''
  print('\n' + ':'*30 + ' ПАРАМЕТРЫ СБОРКИ ' + ':'*30)
  #print(env)
  print(f'- Автор билда: {in_env(env, 'build_author', txt_color=TXT_GREEN)}')
  print(f'- Файл билда: {in_env(env, 'executable_path')}')
  print(f'- Потоков сборки: {in_env(env, 'j')}')
  print(f'- Битность системы: {in_env(env, 'bitness')}')
  print(f'- Система хоста: {in_env(env, 'system')}')
  print(f'- Хост-фреймворк: {in_env(env, 'host')}')
  print(f'- CXX: {in_env(env, 'CXX', none_color=TXT_GRAY, none_val='default')}')
  print(f'- CC: {in_env(env, 'CC', none_color=TXT_GRAY, none_val='default')}')
  #print(f'- Версия SCons: {in_env(env, 'scons_ver')}')
  #print(f'- Версия CXX компилятора: {in_env(env, 'compiler_ver')}')
  print(f'- Версия Python: {in_env(env, 'python_ver')}')
  print(f'- Целевая система: {in_env(env, 'target')}')
  print(f'- Версия игры: {in_env(env, 'game_ver')}')
  print(f'- Последний коммит: дата {in_env(env, 'commit_date')}, время {in_env(env, 'commit_time')}')
  print(f'- ASAN: {checkbox(env, 'asan')}')
  print(f'- Тесты: {in_env(env, 'tests', none_color=TXT_GRAY, none_val='нет')}')
  print(f'- Выключенные опции: {in_env(env, 'disabled', none_color=TXT_GRAY, none_val='нет')}')
  print(f'- Дополнительный софт: {in_env(env, 'soft', none_color=TXT_GRAY, none_val='нет')}')
  print(f'- Начало билда: дата {in_env(env, 'generation_date')}, время {in_env(env, 'generation_time')}')
  if 'info' in env and env['info'] == True:
    quit(txt_yellow('остановка билда (--info)'))

def safe_env_val(env, val):
  if val in env and env[val] != None:
    return env[val]
  return None

def save_json(env, fname):
  try:
    os.makedirs(os.path.dirname(fname), exist_ok=True)

    with open(fname, 'w') as file:
      config = {
        "build": {
          "author": safe_env_val(env, 'build_author'),
          "date": safe_env_val(env, 'generation_date'),
          "time": safe_env_val(env, 'generation_time'),
          "tests": safe_env_val(env, 'tests'),
          "system": {
            "name": safe_env_val(env, 'system'),
            "bitness": safe_env_val(env, 'bitness'),
            "target": safe_env_val(env, 'target'),
          },
          "compiler": {
            "CXX": safe_env_val(env, 'CXX'),
            "CC": safe_env_val(env, 'CC'),
            "compiler_ver": safe_env_val(env, 'compiler_ver'),
            "python_ver": safe_env_val(env, 'python_ver'),
            "scons_ver": safe_env_val(env, 'scons_ver'),
            "asan": safe_env_val(env, 'asan'),
          }
        },
        "game": {
          "version": safe_env_val(env, 'game_ver'),
          "commit_date": safe_env_val(env, 'commit_date'),
          "commit_time": safe_env_val(env, 'commit_time'),
          "disabled": safe_env_val(env, 'disabled'),
          "host_framework": safe_env_val(env, 'host'),
        },
        "checksum": {
          "exe": safe_env_val(env, 'exe_sha256'),
          "data": safe_env_val(env, 'data_sha256'),
        },
      } # JSON info
      json.dump(config, file, indent=2)
      print(txt_green(f'\"{fname}\" saved'))
  except Exception as e:
    print(txt_red(f'Error while savig file {fname}: {e}'))
