'''Глаыный скрипт сборки: py builder'''

import sys
from actions.compile.compile_legacy import compile_legacy
from actions.prepare_build import *
from actions.prepare_info import *
from actions.accept_args import *
from actions.clean import *
from structs.context import *
from structs.target import *
from structs.host import *
from utils.timestamp import utc_time
from utils.ui import *


if __name__ != "__main__":
  print(to_red('Запускать через "python builder"'), file=sys.stderr)
  sys.exit(1)

# подготовка
host = prepare_host_info()
ctx = Context()
tgt = Target()
tgt.name = 'HPW' + tgt.ext
tgt.creation_time = utc_time()

# применяем аргументы запуска
accept_args(tgt, ctx, host)

if ctx.clear_all:
  print('Очистка от файлов сборки')
  clean(ctx)
  print(to_gray('Выход...'))
  sys.exit(0)

prepare_build(ctx)

# показать инфу о сборке
if ctx.with_print_build_info:
  print_build_info(tgt, ctx, host)

if ctx.with_compilation:
  compile_legacy(tgt, ctx, host)

# TODO - пакуем ассеты
# TODO - обработка хэшей

# сейвим инфу о сборке
if ctx.with_build_info_file:
  save_build_info(tgt, ctx, host)
