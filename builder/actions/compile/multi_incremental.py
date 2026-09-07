from actions.compile.multi import compile_multi
from structs.rebuild import Rebuild_info
from structs.context import Context
from structs.host import Host
from structs.target import *
from utils.misc import prepare_obj_name
from utils.hash import blake2b_fast
from utils.ui import *
from utils import fs
import json
import copy
import re

def find_headers(cxx_file: str, include_dirs=None, cache=None, lines=70):
  """
  Рекурсивно ищет инклуды. 
  
  Параметры:
  ---
  lines:
    столько первых строк файла проверять на инклуды
  cache:
    словарь { abs_path: set(прямых_инклудов) } для исключения повторного чтения с диска.
  """
  if cache is None:
    cache = {}
    
  if include_dirs is None:
    include_dirs = []

  cxx_file_abs = fs.path_abs(cxx_file)
  
  # если базы нету
  if not fs.exists(cxx_file_abs):
    return set()

  # Если файл уже в кэше
  if cxx_file_abs in cache:
    return cache[cxx_file_abs]

  # Чтобы не было рекурсии
  cache[cxx_file_abs] = set()
  
  local_dependencies = set()
  include_regex = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
  base_dir = fs.file_dir(cxx_file_abs)
    
  try:
    with open(cxx_file_abs, 'r', encoding='utf-8', errors='ignore') as f:
      for _ in range(lines):
        line = f.readline()
        if not line: 
          break

        # нашли инклуд
        match = include_regex.match(line)
        if match:
          header_name = match.group(1)
          header_path = None

          # ищем локально
          local_path = fs.path_abs(f'{base_dir}/{header_name}')
          if fs.exists(local_path):
            header_path = local_path
          else: # ищем в include_dirs
            for d in include_dirs:
              possible_path = fs.path_abs(f'{d}/{header_name}')
              if fs.exists(possible_path):
                header_path = possible_path
                break

          if header_path:
            local_dependencies.add(header_path)
  except OSError:
    pass

  # Дерево глубоких зависимостей
  full_dependencies = set(local_dependencies)
  for dep in local_dependencies:
    inner_deps = find_headers(dep, include_dirs, cache, lines)
    full_dependencies.update(inner_deps)

  cache[cxx_file_abs] = full_dependencies
  return full_dependencies

def make_header_map(cxx_files: list[str], include_dirs=None):
  if not cxx_files:
    raise ValueError('Пустой список файлов')
  
  # Оптимизация, сразу конвертим в амсолютные пути
  include_dirs = [fs.path_abs(d) for d in (include_dirs or [])]
    
  header_map = {}
  cache = {} # Кэш для файлов проекта
  
  for cxx in cxx_files:
    dependencies = find_headers(cxx, include_dirs=include_dirs, cache=cache)
    header_map[cxx] = { 'headers': list(dependencies) }
    
  return header_map

def make_db(header_map: dict, tgt: Target, ctx: Context):
  '''Создаёт более подробную структуру файлов проекта с их хэшами'''
  db = {}
  db['source'] = header_map

  for cxx_path, content in db['source'].items():
    content['hash'] = blake2b_fast(cxx_path)
    content['obj'] = fs.path_abs(f'{ctx.obj_dir}{prepare_obj_name(cxx_path)}')

    new_headers = {}
    for header in content['headers']:
      new_headers[header] = {
        'hash': blake2b_fast(header),
      }
    content['headers'] = new_headers

  db['opts'] = {
    "opt_preset": tgt.opt_preset,
    "options": tgt.options,
    "name": tgt.name,
    "defines": tgt.defines,
    "include_dirs": tgt.include_dirs,
    "lib_dirs": tgt.lib_dirs,
    "linked_libs": tgt.linked_libs,
    "use_openmp": tgt.use_openmp,
    "author": ctx.author,
    "bin_dir": ctx.bin_dir,
    "obj_dir": ctx.obj_dir,
    "src_dir": ctx.src_dir,
    "tmp_dir": ctx.tmp_dir,
    "compiler_path": ctx.compiler_path,
  }

  # инфа о PCH
  db['pch'] = {
    'used': bool(ctx.pch_path)
  }
  if bool(ctx.pch_path):
    db['pch']['hash'] = blake2b_fast(ctx.pch_path)

  return db

def check_diffs(tgt: Target, db_path: str, header_map: dict, ctx: Context) -> Rebuild_info:
  '''Ищет изменения в файлах кода сравнивая предыдущую базу сборки'''
  info = Rebuild_info()
  local_db = make_db(header_map, tgt, ctx)
  
  print(to_gray(f'> загрузка {db_path}...'))
  with open(db_path, "r", encoding="utf-8") as f:
    loaded_db = json.load(f)

  # найти изменения в файлах
  for cxx_path, local_content in local_db['source'].items():
    if cxx_path in loaded_db['source']:
      loaded_content = loaded_db['source'][cxx_path]
      obj_name = fs.path_abs(f'{ctx.obj_dir}{prepare_obj_name(cxx_path)}')
      
      if 'hash' not in loaded_content or \
      loaded_content['hash'] != local_content['hash'] or \
      not fs.exists(obj_name):
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

  # чекнуть что удаляем
  for old_cxx in list(loaded_db['source'].keys()):
    if old_cxx not in loaded_db['source'] or not fs.exists(old_cxx):
      print(to_red(f'обнаружено удаление файла "{old_cxx}"'))
      info.deleted_files.append(old_cxx)
      info.rebuild_needed = True
  info.new_files = [path for path in info.new_files if fs.exists(path)]
  info.modified_files = [path for path in info.modified_files if fs.exists(path)]

  # проверить на объектники-сироты:
  local_objs = []
  for local_content in local_db['source'].values():
    local_objs.append(local_content['obj'])
  current_objs = fs.find(f'{ctx.obj_dir}*.o')
  for obj in current_objs:
    if obj not in local_objs \
    and fs.file_name(obj) != 'game_ver.o': # game_ver.o это .rc файл на винде
      print(to_red(f'Обнанужен ненужный объектник "{obj}"'))
      fs.rem(obj)
      info.rebuild_needed = True

  # проверить что экзешник снесли:
  executable = fs.path_abs(f'{ctx.bin_dir}{tgt.name}')
  if not fs.exists(executable):
    print(f'"{executable}" удалён. Пересборка...')
    info.rebuild_needed = True
  
  if info.rebuild_needed:
    print(to_green(f'> Обновление базы изменений в файлах {db_path}...'))
    with open(db_path, "w", encoding="utf-8") as f:
      json.dump(local_db, f)

  return info

def equal_opts(db_path: str, tgt: Target, ctx: Context) -> bool:
  '''если параметры сборки поменялись, то надо всё пересобрать по новой'''
  with open(db_path, "r", encoding="utf-8") as f:
    db = json.load(f)
    def opts_differ(db, opt, tag, msg):
      if opt != db['opts'][tag]:
        print(msg)
        return True
      else:
        return False
    # ищем хоть одно различие
    if \
      opts_differ(db, tgt.opt_preset, 'opt_preset', 'изменился пресет') or \
      opts_differ(db, tgt.options, 'options', 'изменились опции компиляции') or \
      opts_differ(db, tgt.name, 'name', 'изменилось имя проекта') or \
      opts_differ(db, tgt.defines, 'defines', 'изменились дефайны компиля') or \
      opts_differ(db, tgt.include_dirs, 'include_dirs', 'изменились папки включения') or \
      opts_differ(db, tgt.lib_dirs, 'lib_dirs', 'изменились папки библиотек') or \
      opts_differ(db, tgt.linked_libs, 'linked_libs', 'изменился список линкуемых библиотек') or \
      opts_differ(db, tgt.use_openmp, 'use_openmp', 'изменилась опция OpenMP') or \
      opts_differ(db, ctx.author, 'author', 'изменился автор сборки') or \
      opts_differ(db, ctx.bin_dir, 'bin_dir', 'изменилась папка бинарника') or \
      opts_differ(db, ctx.obj_dir, 'obj_dir', 'изменился путь к объектникам') or \
      opts_differ(db, ctx.src_dir, 'src_dir', 'изменился путь к сурсам') or \
      opts_differ(db, ctx.tmp_dir, 'tmp_dir', 'изменился путь tmp-папки') or \
      opts_differ(db, ctx.compiler_path, 'compiler_path', 'изменился компилятор'):
      return False
    
  return True

def pch_modifed(db_path: str, ctx: Context):
  ''':resut: True, если есть изменения в pch.hpp'''
  if not bool(ctx.pch_path):
    return False # значит не юзаем PCH
  
  try:
    with open(db_path, "r", encoding="utf-8") as f:
      db = json.load(f)

      pch_node = db['pch']
      if not pch_node['used']:
        return False
      
      # проверяем хэши
      if pch_node['hash'] != blake2b_fast(ctx.pch_path):
        print(f'Обнаружены изменения в {ctx.pch_path}')
        return True
  except FileNotFoundError: # если не нашли в базе, значит ещё не компилили PCH
    return True

  return False

def check_for_rebuild(tgt: Target, header_map: dict, ctx: Context) -> Rebuild_info:
  '''
  Ищет изменения в файлах проекта и определяет что нужно пересобрать.
  
  (+ сохраняет промежуточный результат в файле)

  :return: Инфа - кого пересобирать
  '''
  
  db_path = f'{ctx.tmp_dir}{tgt.name}.json'

  pch_is_modified = pch_modifed(db_path, ctx)

  # проверить что есть база и что опции не изменились
  if fs.exists(db_path) \
  and not pch_is_modified \
  and equal_opts(db_path, tgt, ctx):
    return check_diffs(tgt, db_path, header_map, ctx)
  
  else: # делаем базу с нуля
    print(f'База для пересборки {to_yellow(tgt.name)} не найдена')

    print(to_green(f'> Создание базы изменений в файлах {db_path}...'))
    db = make_db(header_map, tgt, ctx)
    with open(db_path, "w", encoding="utf-8") as f:
      json.dump(db, f)

    info = Rebuild_info()
    info.rebuild_needed = True
    info.rebuild_pch = pch_is_modified
    info.new_files = list(header_map.keys())

    return info

def compile_multi_incremental(tgt_src: Target, ctx: Context, host: Host) -> Rebuild_info:
  ''' Компилирует в многопотоке только изменённые файлы кода '''
  abs_existing_sources = [fs.path_abs(src) for src in tgt_src.sources if fs.exists(src)]
  
  if not abs_existing_sources:
    header_map = {}
  else:
    header_map = make_header_map(abs_existing_sources, include_dirs=tgt_src.include_dirs)
    
  rebuild = check_for_rebuild(tgt_src, header_map, ctx)

  files_to_build = rebuild.modified_files + rebuild.new_files
  if rebuild.rebuild_needed or ctx.forced_rebuild:
    tgt = copy.deepcopy(tgt_src)
    tgt.sources = files_to_build
    if rebuild.rebuild_pch:
      tgt.pch_path = ctx.pch_path
    compile_multi(tgt, ctx, host)
  else:
    print(to_gray(f'Пересборка {tgt_src.name} не требуется'))

  return rebuild
