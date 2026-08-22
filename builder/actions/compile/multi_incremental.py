from utils.ui import *
import utils.fs as fs
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

def make_header_map(search_dir: str):
  """
  Строит карту связей между С++ файлами
  
  :result: чё-то типа:
  {
    ('subdir/a.cxx', {'headers': ['subdir/a.hpp', 'subdir/b.hpp']})
    ('root.c'      , {'headers': ['root.h', 'subdir/a.hpp', 'subdir/b.hpp']})
  }
  """

  cxx_files = []
  cxx_files.extend(fs.find(f'{search_dir}**/*.cxx'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.cpp'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.cc'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.c'))

  if cxx_files == []:
    raise FileNotFoundError('Не удалось найти зависимости между .cpp/.hpp файлами ' \
      f'в папке "{search_dir}"')
  
  map = {}
  for cxx in cxx_files:
    map[cxx] = { 'headers': find_headers(cxx) }

  return map
