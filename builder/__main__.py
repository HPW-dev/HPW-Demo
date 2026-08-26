#!/usr/bin/env python
'''Глаыный скрипт сборки: py builder'''

from structs.target import Extention
from structs.host import Sys_name
from structs.host import Bitness
import utils.fs as fs
import signal
import sys
import os
import subprocess
from actions.compile.compile_legacy import compile_legacy
from actions.prepare_assets import *
from actions.prepare_build import *
from actions.prepare_info import *
from actions.accept_args import *
from structs.context import *
from structs.target import *
from actions.clean import *
from structs.host import *
from utils.timestamp import utc_time
from utils.exec import exec_cmd
from utils.ui import *


def init():
  '''Проверки и колбэки на Ctrl+C'''

  # accept unicode
  if sys.platform == "win32":
    subprocess.run("chcp 65001", shell=True,
      stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
  sys.stdout.reconfigure(encoding='utf-8')
  sys.stderr.reconfigure(encoding='utf-8')

  if __name__ != "__main__":
    print(to_red('Запускать через "python builder"'), file=sys.stderr)
    sys.exit(1)

  def signal_handler(sig, frame):
    '''Для прерывания по Ctrl+C'''
    try:
      sig_name = signal.Signals(sig).name
    except ValueError:
      sig_name = f"SIG-{sig}"
    print(to_red(f"\n> Сборка прервана сигналом {sig_name}"), file=sys.stderr)
    os._exit(1)

  signal.signal(signal.SIGINT, signal_handler)

# подготовка
init()
host = prepare_host_info()
ctx = Context()
tgt = Target()
if host.system == Sys_name.windows:
  tgt.ext = Extention.exe
elif host.system == Sys_name.linux:
  tgt.ext = Extention.elf64 if host.bitness == Bitness.x64 else Extention.elf32
tgt.name = 'HPW' + tgt.ext
tgt.creation_time = utc_time()

# применяем аргументы запуска
accept_args(tgt, ctx, host)

if ctx.clear_all:
  print('Очистка от файлов сборки')
  clean(ctx)
  sys.exit(0)

prepare_build(ctx)

# показать инфу о сборке
if ctx.with_print_build_info:
  print_build_info(tgt, ctx, host)

if ctx.with_compilation:
  compile_legacy(tgt, ctx, host)

if ctx.with_assets:
  prepare_assets(tgt, ctx, host)

# сейвим инфу о сборке
if ctx.with_build_info_file:
  save_build_info(tgt, ctx, host)

game_exe = fs.path_abs(f'{ctx.bin_dir}{tgt.name}')
if ctx.with_launch and fs.exists(game_exe):
  print(f'> запуск "{game_exe}"...')
  exec_cmd(game_exe)
