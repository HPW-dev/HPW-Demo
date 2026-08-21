from zlib import crc32
from structs.context import *
from structs.target import *
from structs.host import *
from utils.ui import *
from utils.fs import *
from utils.hash import crc32
import pickle
from sys import stderr


type Diff_data = dict[str, tuple[float, int]]

def check_file_diffs(folder: str, cache_path: str) -> bool:
  """проверяет что в папке folder изменился хотя бы один файл"""
  
  # Загружаем базу кэша
  try:
    with open(cache_path, 'rb') as file:
      diff_data: Diff_data = pickle.load(file)
  except (FileNotFoundError, pickle.UnpicklingError):
    return True # Если кэш поврежден или пропал — папка изменилась

  folder_path = Path(folder)
  scanned_paths = set()

  # Проходимся по всем текущим файлам в папке
  for p in folder_path.rglob('*'):
    if not p.is_file():
      continue
      
    file_path_str = str(p)
    scanned_paths.add(file_path_str)

    # Если файл уже был в базе
    if file_path_str in diff_data:
      time_info, file_checksum = diff_data[file_path_str]
      stat = p.stat()
      
      # Сравниваем время модификации
      if stat.st_mtime != time_info:
        # Если время не совпало, проверяем реальную чексумму файла
        if crc32(file_path_str) != file_checksum:
          print(f'обнаружены изменения в файле "{file_path_str}"')
          return True
    else:
      # Новый файл, которого не было в базе
      print(to_green(f'добавлен новый файл "{file_path_str}"'))
      return True
  
  # Проверяем, не удалили ли какие-то файлы из базы
  for file_path_str in diff_data:
    if file_path_str not in scanned_paths:
      print(to_magenta(f'обнаружено удаление файла "{file_path_str}"'))
      return True

  return False

def make_diff_data(folder: str, cache_path: str):
  """сейвит базу изменений в файлах в cache_path"""
  diff_data = {
    str(p): (p.stat().st_mtime, crc32(str(p)))
    for p in Path(folder).rglob('*') if p.is_file()
  }
  Path(cache_path).write_bytes(pickle.dumps(diff_data, protocol=pickle.HIGHEST_PROTOCOL))

def compress_folder(folder: str, archive_path: str):
  """жмёт всю папку folder в архив archive_path"""
  path = Path(archive_path).with_suffix('')
  shutil.make_archive(str(path), 'zip', folder)

def compress_diffs(folder: str, archive_path: str, cache_path: str) -> bool:
  """Отслеживает изменения в папке и сжимает её, если изменения были"""
  try:
    if not folder:
      raise ValueError("не указана папка для отслеживания изменений")
    if not archive_path:
      raise ValueError("не указан итоговый файл архива ресурсов")
    if not cache_path:
      raise ValueError("укажите путь к базе изменений в файлах")

    folder_path = Path(folder)
    if not folder_path.is_dir():
      raise FileNotFoundError(f'папка "{folder}" не обнаружена')
    if not any(folder_path.iterdir()):
      raise ValueError(f'папка "{folder}" пуста')

    need_archive = False
    cache_file = Path(cache_path)

    if not cache_file.is_file():
      print(f'Нету базы изменений в файлах в "{cache_path}".')
      need_archive = True
    elif check_file_diffs(folder, cache_path):
      print(to_green(f'в папке "{folder}" есть изменения.'))
      need_archive = True

    if need_archive:
      print(to_green(f'создание базы изменений в файлах в "{cache_path}"'))
      make_diff_data(folder, cache_path)
      print(to_green(f'создание архива папки "{folder}" в "{archive_path}"'))
      compress_folder(folder, archive_path)
    else:
      print(to_gray(f'изменений в "{folder}" нет, действия не требуются.'))
      
    return True # ИСПРАВЛЕНО: теперь возвращает True в конце успешного блока try, а не падает в пустой else
  except Exception as ex:
    print(to_red(f'Ошибка при отслеживании изменений в ресурсах игры:\n* {ex}'), file=stderr)
    return False

def prepare_assets(tgt: Target, ctx: Context, host: Host):
  '''Подготовка архива с ресурсами игры'''
  compress_diffs(ctx.assets_src_dir, ctx.assets_dst_path, f'{ctx.tmp_dir}resources_diff.pickle')
