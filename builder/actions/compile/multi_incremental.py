from utils.ui import *
import utils.fs as fs

def find_headers(cxx_file: str):
  result = []
  return result

def make_header_map(search_dir: str):
  '''Строит карту связей между .cpp/.c и .hpp/.h'''

  cxx_files = []
  cxx_files.extend(fs.find(f'{search_dir}**/*.cxx'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.cpp'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.cc'))
  cxx_files.extend(fs.find(f'{search_dir}**/*.c'))
  if cxx_files == []:
    raise ValueError('Не удалось найти зависимости между .cpp/.hpp файлами ' \
      f'в папке "{search_dir}"')
  
  map = {}
  for cxx in cxx_files:
    map[cxx] = { 'headers': find_headers(cxx) }

  return map