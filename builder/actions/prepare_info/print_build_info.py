from structs.context import *
from structs.target import *
from structs.host import *
from utils.ui import *


def print_build_info(tgt: Target, ctx: Context, host: Host):
  '''показывает сводку билда'''

  print('=== Сводка билда ===')
  print(f'* Итоговый файл .... {to_yellow(tgt.name)}')
  print(f'* Автор сборки ..... {to_yellow(ctx.author)}')
  print(f'* Время старта ..... {to_yellow(tgt.creation_time)}')
  print(f'* Компиляция ....... {checkbox2(ctx.with_compilation)}')
  print(f'* Паковать ассеты .. {checkbox2(ctx.with_assets)}')
  print(f'* OpenMP ........... {checkbox2(tgt.use_openmp)}')
  print()

  print('=== Информация системы-сборщика ===')
  print(f'* Система .......... {to_yellow(host.system)}')
  print(f'* Разрядность ...... {to_yellow(host.bitness)}')
  print(f'* Потоков .......... {to_yellow(host.threads)}')
  print()

  print('=== Информация о компиляторе ===')
  print(f'* Временные файлы .. {to_yellow(ctx.tmp_dir)}')
  print(f'* Потоки сборки .... {to_yellow(ctx.threads)}')
  print(f'* Версия питона .... {to_yellow(host.python_ver.rstrip())}')
  print(f'* Компилятор ....... {to_yellow(ctx.compiler_path)}')
  print(f'* Версия компиля ... {to_yellow(translate_none(host.compiler_ver).rstrip())}')
  print(f'* CXX OPTS ......... {to_yellow(' '.join(tgt.options))}')
  print(f'* CXX DEFINES ...... {to_yellow(' '.join(tgt.defines))}')
  print(f'* CXX LINKED LIBS .. {to_yellow(' '.join(tgt.linked_libs))}')
  print()
