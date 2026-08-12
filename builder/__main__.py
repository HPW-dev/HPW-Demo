'''Глаыный скрипт сборки: py builder'''

import sys
from actions.accept_args import *
from actions.prepare_info import *
from actions.prepare_build import *
from utils.ui import *
from utils.timestamp import utc_time
from structs.host import *
from structs.context import *
from structs.target import *


if __name__ != "__main__":
  print(to_red('Запускать через "python builder"'), file=sys.stderr)
  sys.exit(1)

# подготовка
host = prepare_host_info()
ctx = Context()
tgt = Target()
tgt.creation_time = utc_time()

accept_args(tgt, ctx, host)
prepare_build(ctx)

# показать инфу о сборке
if ctx.with_print_build_info:
  print_build_info(tgt, ctx, host)

# TODO - применяем параметры к контексту, таргету и прочему
# TODO - пишем что будет сделано
# TODO - подготавливаем билд, если нужно
# TODO - собираем нужное
# TODO - пакуем ассеты

# сейвим инфу о сборке
if ctx.with_build_info_file:
  save_build_info(tgt, ctx, host)
