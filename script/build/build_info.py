if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

from script.build.pretty_txt import *
import json

def print_info(env):
  '''показывает сводку о параметрах билда'''
  print(':'*25 + ' ПАРАМЕТРЫ СБОРКИ ' + ':'*25)
  #print(env)
  print(f'- CXX: {in_env(env, 'CXX', none_color=TXT_GRAY, none_val='default')}')
  print(f'- CC: {in_env(env, 'CC', none_color=TXT_GRAY, none_val='default')}')
  print(f'- Потоков сборки: {in_env(env, 'j')}')
  print(f'- Битность системы: {in_env(env, 'bitness')}')
  print(f'- Система хоста: {in_env(env, 'system')}')
  print(f'- Хост-фреймворк: {in_env(env, 'host')}')
  #print(f'- Версия SCons: {in_env(env, 'scons_ver')}')
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
    with open(fname, 'w') as file:
      config = {
        "CXX": safe_env_val(env, 'CXX'),
        "CC": safe_env_val(env, 'CC'),
        "use_asan": safe_env_val(env, 'asan'),
        "game_ver": safe_env_val(env, 'game_ver'),
        "commit_date": safe_env_val(env, 'commit_date'),
        "commit_time": safe_env_val(env, 'commit_time'),
        "python_ver": safe_env_val(env, 'python_ver'),
        "scons_ver": safe_env_val(env, 'scons_ver'),
        "tests": safe_env_val(env, 'tests'),
        "disabled": safe_env_val(env, 'disabled'),
        "host_framework": safe_env_val(env, 'host'),
        "system": safe_env_val(env, 'system'),
        "bitness": safe_env_val(env, 'bitness'),
        "target": safe_env_val(env, 'target'),
        "generation_date": safe_env_val(env, 'generation_date'),
        "generation_time": safe_env_val(env, 'generation_time'),
      }
      json.dump(config, file, indent=2)
  except Exception as e:
    print(txt_red(f'Error while savig file {fname}: {e}'))
