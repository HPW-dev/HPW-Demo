from structs.rebuild import Rebuild_info
from structs.context import Context
from structs.host import Host
from structs.target import *
from actions.compile.multi import compile_multi
from utils.ui import *
from utils.hash import blake2b
import utils.fs as fs
import json
import re

def find_headers(cxx_file: str, visited=None, lines=200):
  '''
  Рекурсивно лазит по инклудам и добавляет в список всё что между кавычек "..."

  :lines: весь файл не читать, только первые строки
  '''

  if visited is None:
    visited = set()
        
  # Предотвращаем бесконечную рекурсию при циклических инклудах
  if cxx_file in visited or not fs.exists(cxx_file):
    return []
    
  visited.add(cxx_file)
  headers = []
  include_regex = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
  base_dir = fs.file_dir(cxx_file)
    
  try:
    with open(cxx_file, 'r', encoding='utf-8', errors='ignore') as f:
      for _ in range(lines):
        line = f.readline()
        if not line: # EOF
          break
                
        match = include_regex.match(line)
        if match:
          header_name = match.group(1)
          header_path = fs.path_abs(f'{base_dir}/{header_name}')
                    
          if header_path not in visited:
            headers.append(header_path)
            inner_headers = find_headers(header_path, visited)
            headers.extend(inner_headers)

  except IOError:
    pass
        
  return headers

def make_header_map(cxx_files: list[str]):
  """
  Строит карту связей между С++ файлами
  
  :result: чё-то типа:
  {
    ('subdir/a.cxx', {'headers': ['subdir/a.hpp', 'subdir/b.hpp']})
    ('root.c'      , {'headers': ['root.h', 'subdir/a.hpp', 'subdir/b.hpp']})
  }
  """
  if cxx_files == []:
    raise ValueError('Пустой список файлов')
  map = {}
  for cxx in cxx_files:
    map[cxx] = { 'headers': find_headers(cxx) }
  return map

def make_db(header_map: dict):
  '''Создаёт более подробную структуру файлов проекта с их хэшами'''
  db = header_map

  for cxx_path, content in db.items():
    content['c++ hash'] = blake2b(cxx_path)

    new_headers = []
    for header in content['headers']:
      header_content = {
        'path': header,
        'hash': blake2b(header),
      }
      new_headers.append(header_content)
    content['headers'] = new_headers

  return db

def check_diffs(db_path: str, header_map: dict, ctx: Context) -> Rebuild_info:
  '''Ищет изменения в файлах кода сравнивая предыдущую базу сборки'''
  info = Rebuild_info()
  local_db = make_db(header_map)

  print(to_gray(f'> загрузка {db_path}...'))
  loaded_db = {}
  with open(db_path, "r", encoding="utf-8") as f:
    loaded_db = json.load(f)

  if info.rebuild_needed:
    print(to_green(f'> Обновление базы изменений в файлах {db_path}...'))
    with open(db_path, "w", encoding="utf-8") as f:
      json.dump(local_db, f, indent=2)

  return info

def check_for_rebuild(target_name: str, header_map: dict, ctx: Context) -> Rebuild_info:
  '''
  Ищет изменения в файлах проекта и определяет что нужно пересобрать.
  
  (+ сохраняет промежуточный результат в файле)

  :return: Инфа - кого пересобирать
  '''
  
  # ищем базу для пересборки
  db_path = f'{ctx.tmp_dir}target_name.json'

  # если нашли, то сравниваем различия
  if fs.exists(db_path):
    return check_diffs(db_path, header_map, ctx)
    
  # если не нашли, создаём новую базу и врубаем принудительную пересборку
  else:
    print(f'База для пересборки {to_yellow(target_name)} не найдена')

    print(to_green(f'> Создание базы изменений в файлах {db_path}...'))
    db = make_db(header_map)
    with open(db_path, "w", encoding="utf-8") as f:
      json.dump(db, f, indent=2)

    info = Rebuild_info()
    info.rebuild_needed = True
    info.new_files = list(header_map.keys())

    return info

def compile_multi_incremental(tgt_src: Target, ctx: Context, host: Host) -> Rebuild_info:
  ''' Компилирует в многопотоке только изменённые файлы кода '''
  rebuild = check_for_rebuild(tgt_src.name, make_header_map(tgt_src.sources), ctx)

  if rebuild.rebuild_needed:
    tgt = tgt_src
    tgt.sources.clear()
    tgt.sources.extend(rebuild.modified_files)
    tgt.sources.extend(rebuild.new_files)
    compile_multi(tgt, ctx, host)

  else:
    print(to_gray(f'Пересборка {tgt_src.name} не требуется'))

  return rebuild
