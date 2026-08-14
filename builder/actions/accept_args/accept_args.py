from structs.target import *
from structs.context import *
from structs.host import *
import argparse


def accept_args(tgt: Target, ctx: Context, host: Host):
  '''Применяем аргументы запуска'''

  parser = argparse.ArgumentParser(
    usage='py builder --author "YOUR_NICK"'
  )

  parser.add_argument(
    '-a', '--author', 
    type=str, default='Unknown',
    help='Никнейм автора сборки (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-t', '--threads', 
    type=int, default=host.threads,
    help='Число потоков для сборки (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-tmp', '--tmp_dir', 
    type=str, default=ctx.tmp_dir,
    help='Папка для временных файлов сборки (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-bin', '--bin_dir', 
    type=str, default=ctx.bin_dir,
    help='Куда создать экзешник (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-bdir', '--build_dir', 
    type=str, default=ctx.build_dir,
    help='Общая папка для билда (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-odir', '--obj_dir', 
    type=str, default=ctx.obj_dir,
    help='Путь для объектных файлов компилятора (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-idir', '--info_dir', 
    type=str, default=ctx.info_dir,
    help='Куда копировать сводку билда (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-cxx', '--cxx_dir', 
    type=str, default=ctx.compiler_path,
    help='Компилятор/Путь до него (по умолчанию: %(default)s)'
  )
  parser.add_argument(
    '-np', '--no_print', 
    action='store_true', 
    help='Выключает отображение итоговой сводки билда'
  )
  parser.add_argument(
    '-ni', '--no_info', 
    action='store_true', 
    help='Выключает сохранение инфы о билде'
  )
  parser.add_argument(
    '-nl', '--no_license', 
    action='store_true', 
    help='Выключает копирование лицензий'
  )
  parser.add_argument(
    '-nc', '--no_compilation', 
    action='store_true', 
    help='Выключает компиляцию'
  )
  parser.add_argument(
    '-nmp', '--no_openmp', 
    action='store_true', 
    help='Выключает OpenMP'
  )

  args = parser.parse_args()

  if args.no_print:
    ctx.with_print_build_info = False
  if args.no_info:
    ctx.with_build_info_file = False
  if args.no_license:
    ctx.with_licenses = False
  if args.no_compilation:
    ctx.with_compilation = False
  if args.no_openmp:
    tgt.use_openmp = False
  ctx.compiler_path = args.cxx_dir
  ctx.info_dir = args.info_dir
  ctx.build_dir = args.build_dir
  ctx.tmp_dir = args.tmp_dir
  ctx.bin_dir = args.bin_dir
  ctx.obj_dir = args.obj_dir

  return args
  