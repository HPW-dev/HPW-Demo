from actions.exec_multi import *
from actions.prepare_game_ver import game_version
from structs.context import *
from structs.target import *
from structs.host import *
from utils.misc import prepare_obj_name
from utils.exec import exec_cmd
from utils.ui import *
from utils import fs
from sys import stderr
import time


def prepare_obj_cmd(tgt: Target, ctx: Context):
  '''генерит команду на компиляцию .o файла,
  которая потом будет выполняться в многопотоке'''
  
  fs.make_dir(ctx.obj_dir)
  cmds = []

  for cxx_src in tgt.sources:
    cmd = [ctx.compiler_path]
    cmd.extend([f'-D{define}' for define in tgt.defines])
    cmd.extend([f'-{opt}' for opt in tgt.options])
    cmd.extend([f'-I{fs.path_abs(path)}' for path in tgt.include_dirs])
    if bool(tgt.pch_path):
      cmd.append(f'-I{fs.file_dir(tgt.pch_path)}')
    cmd.extend(['-c', cxx_src])
    obj_name = prepare_obj_name(cxx_src)
    cmd.extend(['-o', fs.path_abs(f'{ctx.obj_dir}{obj_name}')])
    cmds.append(cmd)

  return cmds

def make_object_list(ctx: Context) -> str:
  '''создать список объектников, который потом подаётся при линковке через @'''

  result = f'{ctx.obj_dir}objects.txt'
  with open(result, 'w', encoding='utf-8') as f:
    f.writelines(f"{fs.path_linux(obj)}\n" for obj in fs.find(f'{ctx.obj_dir}*.o'))
  return f'@{fs.path_abs(result)}'

def compile_pch(tgt: Target, ctx: Context):
  '''
  Компилирует PCH файл в .gch
  
  Лежать выходные файлы должны там же, где и pch.hpp потому что GCC инвалид
  '''
  fs.make_dir(ctx.obj_dir)

  cmd = [ctx.compiler_path]
  cmd.extend([f'-D{define}' for define in tgt.defines])
  cmd.extend([f'-{opt}' for opt in tgt.options])
  cmd.extend([f'-I{fs.path_abs(path)}' for path in tgt.include_dirs])
  cmd.extend(['-x', 'c++-header', tgt.pch_path])
  exec_cmd(cmd)

  gch_path = f'{fs.file_dir(tgt.pch_path)}/pch.hpp.gch'
  if fs.exists(gch_path):
    print(to_green(f'{gch_path} файл успешно создан'))
  else:
    raise FileNotFoundError(f'Файл "{gch_path}" не создан')

def compile_game_ver(tgt: Target, ctx: Context):
  '''Собирает из ресурс-файла инфу о версии приложения'''
  try:
    if not fs.exists(tgt.game_ver_file):
      raise FileNotFoundError(f'файл "{tgt.game_ver_file}" не найден')

    game_ver_copy = f'{ctx.tmp_dir}version.rc'
    fs.copy(tgt.game_ver_file, game_ver_copy)

    # меняем версию с ресурса на актуальную
    v, _, _ = game_version(ctx)
    if v:
      v = v.replace('v', '')
      comma_ver = v.replace('.', ',')
      point_ver = v.replace(',', '.')
      with open(game_ver_copy, mode='r', encoding='utf-8') as f:
        content = f.read()
      content = content.replace('1,0,0,0', comma_ver)
      content = content.replace('1.0.0.0', point_ver)
      with open(game_ver_copy, mode='w', encoding='utf-8') as f:  
        f.write(content)

    out_path = f'{ctx.obj_dir}game_ver.o'
    fs.rem(out_path)
    exec_cmd(['windres', '--input-format=rc', game_ver_copy, '-o', out_path])

    if not fs.exists(out_path):
      raise FileNotFoundError(f'файл "{out_path}" не сгенерирован')
    
  except Exception as ex:
    print(to_red(f'ошибка при генерации файла версии:\n  {ex}'), file=stderr)

def compile_multi(tgt: Target, ctx: Context, host: Host):
  '''Многопоточная компиляция без инкрементальной сборки'''
  start = time.perf_counter()

  if bool(tgt.pch_path):
    print(f'> компиляция PCH ("{to_yellow(tgt.pch_path)}")...')
    compile_pch(tgt, ctx)

  if bool(tgt.game_ver_file):
    print(f'> компиляция инфы о версии игры...')
    compile_game_ver(tgt, ctx);

  obj_cmds = prepare_obj_cmd(tgt, ctx)
  print('> компиляция объектных файлов...')
  exec_multi(obj_cmds, ctx.threads, 600.0, ctx.less_info)

  print(f'> линковка \'{to_yellow(tgt.name)}\'')
  cmd = [ctx.compiler_path]
  cmd.extend([f'-D{define}' for define in tgt.defines])
  cmd.extend([f'-{opt}' for opt in tgt.options])
  cmd.extend([f'-I{fs.path_abs(path)}' for path in tgt.include_dirs])
  if bool(tgt.pch_path):
    cmd.append(f'-I{fs.file_dir(tgt.pch_path)}')
  cmd.append(make_object_list(ctx))
  cmd.extend([f'-L{fs.path_abs(path)}' for path in tgt.lib_dirs])
  executable = fs.path_abs(f'{ctx.bin_dir}{tgt.name}')
  cmd.extend(['-o', executable])
  cmd.extend(tgt.linked_libs)
  print(to_yellow(' '.join(cmd)))
  exec_cmd(cmd, 600.0)

  if not fs.exists(executable):
    raise FileExistsError(f'не удалось создать файл \'{executable}\'')

  end = time.perf_counter()
  elapsed = round(end - start, 2)
  print(to_gray(f'Общее время компиляции: {elapsed} сек.'))
