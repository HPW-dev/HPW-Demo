import utils.fs as fs
from utils.ui import *
from structs.context import *


def make_if_not_exists(path: str):
  '''делает папку, если её нету + лог'''
  if not fs.exists(path):
    fs.make_dir(path)
    print(f'Создана папка \"{path}\"')

def check_dir(dirs: list [str]):
  '''если папки нет - взрыв'''
  for dir in dirs:
    if not fs.exists(dir):
      raise FileExistsError(f'не найдена папка \"{dir}\"')

def copy(src: str, dst: str):
  fs.copy(src, dst)
  print(to_gray(f'\"{src}\" скопирован в \"{dst}\"'))

def prepare_build(ctx: Context):
  '''подготавливаем всё к сборке игры'''
  print(to_gray(f'проврка файлов для сборки...'))
  make_if_not_exists(ctx.build_dir)
  make_if_not_exists(ctx.bin_dir)
  make_if_not_exists(ctx.info_dir)
  check_dir(['src', 'thirdparty', 'data', 'thirdparty'])

  # copyright
  if ctx.with_licenses:
    copy('LICENSE.txt', ctx.info_dir)
    copy('NOTICE.txt', ctx.info_dir)
  if ctx.with_print_build_info:
    pass # TODO