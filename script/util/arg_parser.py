import argparse

def parse_args():
  ''' Парсит команды запуска '''
  parser = argparse.ArgumentParser (
    prog='python -m script.build',
    description='Script for build H.P.W',
    epilog='Copyright (c) 2020-2025 HPW-dev <hpwdev0@gmail.com>',
  )
  parser.add_argument('-t', '--threads', type=int, default=0, help='custom CPU\'s thread number for building')
  parser.add_argument('-i', '--info', action='store_true', help='disable building (use for watching options)')
  parser.add_argument('-p', '--platform', type=str, help='supported platforms: win32, win64, lin64')
  parser.add_argument('-c', '--cpu', type=str, default='x86-64', help='supported CPU\'s: pentium2, atom, core2, x86-64, x86-64-v4')
  parser.add_argument('-o', '--omp', default=True, action='store_true', help='use OpenMP multithreading')
  parser.add_argument('-a', '--asan', default=False, action='store_true', help='use ASAN profiler')
  args = parser.parse_args()
  #print(args)
  parser.print_help()
  return args

def prepare_config():
  ''' Подготавливает команды для конфига '''
  args = parse_args()
  return None
  
