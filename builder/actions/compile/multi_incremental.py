from structs.rebuild import Rebuild_info
from structs.context import Context
from structs.host import Host
from structs.target import *
from actions.compile.multi import compile_multi
from utils.ui import *
from utils.hash import blake2b
import utils.fs as fs
import json
import copy
import re

def find_headers(cxx_file: str, visited=None, lines=200):
  '''
  Рекурсивно лазит по инклудам и добавляет в список всё что между кавычек "..."

  :lines: весь файл не читать, только первые строки
  '''

  if visited is None:
    visited = set()
        
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
    content['hash'] = blake2b(cxx_path)

    new_headers = {}
    for header in content['headers']:
      new_headers[header] = {
        'hash': blake2b(header),
      }
    content['headers'] = new_headers

  return db

def check_diffs(db_path: str, header_map: dict, ctx: Context) -> Rebuild_info:
  '''Ищет изменения в файлах кода сравнивая предыдущую базу сборки'''
  info = Rebuild_info()
  local_db = make_db(header_map)
  
  print(to_gray(f'> загрузка {db_path}...'))
  with open(db_path, "r", encoding="utf-8") as f:
    loaded_db = json.load(f)

  for cxx_path, local_content in local_db.items():
    if cxx_path in loaded_db:
      loaded_content = loaded_db[cxx_path]
      
      if loaded_content['hash'] != local_content['hash']:
        print(to_yellow(f'обнаружено изменение в файле "{cxx_path}"'))
        info.modified_files.append(cxx_path)
        info.rebuild_needed = True
        continue
        
      for lchhp, lchpp_cnt in local_content['headers'].items():
        if lchhp not in loaded_content['headers'] or lchpp_cnt['hash'] != loaded_content['headers'][lchhp]['hash']:
          print(to_green(f'в файле "{cxx_path}" обнаружены изменения in "{lchhp}"'))
          info.modified_files.append(cxx_path)
          info.rebuild_needed = True  
          break 

    else:
      print(to_green(f'обнаружен новый файл "{cxx_path}"'))
      info.new_files.append(cxx_path)
      info.rebuild_needed = True

  for old_cxx in list(loaded_db.keys()):
    if old_cxx not in local_db or not fs.exists(old_cxx):
      print(to_red(f'обнаружено удаление файла "{old_cxx}"'))
      info.deleted_files.append(old_cxx)
      info.rebuild_needed = True
  info.new_files = [path for path in info.new_files if fs.exists(path)]
  info.modified_files = [path for path in info.modified_files if fs.exists(path)]
  
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
  
  db_path = f'{ctx.tmp_dir}{target_name}.json'

  if fs.exists(db_path):
    return check_diffs(db_path, header_map, ctx)
    
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
  abs_existing_sources = [fs.path_abs(src) for src in tgt_src.sources if fs.exists(src)]
  
  if not abs_existing_sources:
    header_map = {}
  else:
    header_map = make_header_map(abs_existing_sources)
    
  rebuild = check_for_rebuild(tgt_src.name, header_map, ctx)

  files_to_build = rebuild.modified_files + rebuild.new_files
  if rebuild.rebuild_needed and files_to_build:
    tgt = copy.deepcopy(tgt_src)
    tgt.sources = files_to_build
    compile_multi(tgt, ctx, host)

  else:
    print(to_gray(f'Пересборка {tgt_src.name} не требуется'))

  return rebuild
