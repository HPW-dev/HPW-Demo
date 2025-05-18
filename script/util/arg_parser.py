import argparse


def parse():
  ''' Парсит команды запуска '''

  parser = argparse.ArgumentParser (
    prog='python -m script.build',
    description='Script for build H.P.W',
    epilog='Copyright (c) 2020-2025 HPW-dev <hpwdev0@gmail.com>',
  )

  parser.add_argument('--threads', type=int, default=0, help='custom CPU\'s thread number for building')
  parser.add_argument('-c', '--clean', type=bool, default=False, help='clean .o/.exe/.dll files aftre compilation')
  parser.add_argument('-i', '--info', action='store_true', help='disable building (use for watching options)')
  parser.add_argument('-t', '--target', type=str, help='supported targets: win_x64_debug win_x32_debig '
    'lin_x64_debug win_xp win_atom win_core2 win_x64 win_x64_v4 lin_x32 lin_x64 lin_x64_v4')
  parser.add_argument('-e', '--enable',  nargs='+', help='enable options: asan omp replay netplay '
    'sound data config static')
  parser.add_argument('--host', type=str, default='glfw3', help='supported hosts: sdl2 glfw3 null asci')
  parser.add_argument('--cxx', type=str, help='default C++ compiler')
  parser.add_argument('--cc', type=str, help='default C compiler')
  parser.add_argument('--tests', nargs='+', help='avaliable tests: graphic math random sound yaml '
    'file-io network')
  parser.add_argument('--launch', nargs='+', help='post-build launch: hpw editor tests')
  
  args = parser.parse_args()
  args = prepare(args)
  parser.print_help()

  return args
  

def prepare(args):
  ''' Подготавливает команды для конфига '''
  args.ld_flags = []
  args.defines = []
  args.cxx_flags = []
  args.system = None
  args.bitness = None
  return args
