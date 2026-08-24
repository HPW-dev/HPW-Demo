from actions.prepare_info import compiler_version
from structs.target import *
from structs.context import *
from structs.host import *
from utils.ui import *
import argparse
import json


def accept_preset(preset_name, tgt: Target, ctx: Context, host: Host):
  with open('builder/actions/accept_args/presets.json', "r", encoding="utf-8") as f:
    # достаём настройки с конфига пресетов
    presets = json.load(f)
    if preset_name == 'auto':
       tgt.opt_preset = preset_name = 'stable-x32' if host.bitness == Bitness.x32 else 'stable-x64'

    preset = presets[tgt.opt_preset]
    # TODO

def accept_args(tgt: Target, ctx: Context, host: Host):
  '''Применяем аргументы запуска'''

  parser = argparse.ArgumentParser(
    usage='for build:\n  python builder --author "YOUR_NICK" --threads 4\n' \
      'for clear:\n  python builder -c'
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
    '-cxx', '--cxx_path', 
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
  parser.add_argument(
    '-c', '--clear', 
    action='store_true', 
    help='Очищает от файлов сборки и прерывает сборку'
  )
  parser.add_argument(
    '-pre', '--preset', 
    type=str, default='auto',
    help='Уровень оптимизации кода (по умолчанию: %(default)s): ' \
      'auto - ставит либо stable-x64, либо x32; ' \
      'stable-x64 - должно работать у всех; ' \
      'stable-x32 - для старья; ' \
      'atom-x32 - Intel Atom x32; ' \
      'c2d-x32 - Core 2 Duo x32; ' \
      'c2d-x64 - Core 2 Duo x64; ' \
      'r1700 - Ryzen 1700 x64 (znver1); ' \
      '2003 - Процы 2000-2003 года, x64, SSE2; ' \
      '2010 - Процы 2008-2010 года, x64, SSE4.2; ' \
      '2015 - Процы 2013-2015 года, x64, AVX2; ' \
      '2020 - Процы 2017-2020+ года, x64, AVX-512; ' \
      'fast-build - быстрая сборка (без дебага, x64); ' \
      'debug-x32 - отладочный билд x32; ' \
      'debug-x64 - отладочный билд x64.'
  )

  args = parser.parse_args()

  ctx.with_print_build_info = not bool(args.no_print)
  ctx.with_build_info_file = not bool(args.no_info)
  ctx.with_licenses = not bool(args.no_license)
  ctx.with_compilation = not bool(args.no_compilation)
  tgt.use_openmp = not bool(args.no_openmp)

  if not args.clear and "CXX" not in host.env:
      if args.cxx_path == ctx.compiler_path and not ctx.compiler_path:
          parser.error("аргумент -cxx/--cxx_path обязателен, " \
            "если не выполняется очистка (-c) и нет переменной среды (CXX)")

  ctx.compiler_path = args.cxx_path
  host.env['CXX'] = str(args.cxx_path)
  host.compiler_ver = compiler_version(host.env)

  ctx.info_dir = args.info_dir
  ctx.build_dir = args.build_dir
  ctx.tmp_dir = args.tmp_dir
  ctx.bin_dir = args.bin_dir
  ctx.obj_dir = args.obj_dir
  ctx.author = args.author
  ctx.threads = max(1, args.threads)
  ctx.clear_all = bool(args.clear)

  tgt.opt_preset = args.preset
  accept_preset(tgt.opt_preset, tgt, ctx, host)

  return args
  