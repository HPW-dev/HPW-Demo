from structs.context import *
from utils.ui import *
from utils.fs import *


def delete_file(path: str):
  if exists(path):
    if is_dir(path):
      print(to_red(f'удаление папки \'{path_abs(path)}\''))
      rem_dir(path)
    else:
      print(to_red(f'удаление файла \'{path_abs(path)}\''))
      rem(path)

def clean(ctx: Context):
  '''Чистит от файлов сборки'''
  # собираем список кого удалить:
  delete_list = []
  delete_list.extend(find(f'{ctx.bin_dir}*.exe'))
  delete_list.append(ctx.info_dir)
  delete_list.extend(find('**/__pycache__'))
  delete_list.extend(find(f'{ctx.tmp_dir}*'))
  delete_list = [item for item in delete_list if not item.endswith('.gitkeep')]  

  if delete_list == []:
    print(to_gray('нечего удалять'))
    return

  for path in delete_list:
    delete_file(path)
