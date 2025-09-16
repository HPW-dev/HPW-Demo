if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

from script.build.color_text import in_env

def print_info(env):
  '''показывает сводку о параметрах билда'''
  #print(env)
  print(':'*25 + ' ПАРАМЕТРЫ СБОРКИ ' + ':'*25)
  print(f'- CXX: {in_env(env, 'cxx')}')
  print(f'- CC: {in_env(env, 'cc')}')
  print(f'- Потоков сборки: {in_env(env, 'j')}')
  #print(f'- Версия SCons: {in_env(env, 'scons_ver')}')
  print(f'- Версия Python: {in_env(env, 'python_ver')}')
  print(f'- Версия игры: {in_env(env, 'game_ver')}')
  print(f'- Последний коммит: дата {in_env(env, 'commit_date')}, время {in_env(env, 'commit_time')}')
