from actions.exec_multi import *
from structs.context import *
from structs.target import *
from structs.host import *
from utils.exec import exec_cmd
from utils.ui import *
from utils import fs
import time


def prepare_obj_name(raw: str) -> str:
  raw = raw.replace(' ', '-')
  raw = raw.replace('/', '_')
  raw = raw.replace('\\', '_')
  raw = raw.replace('.cpp', '')
  raw = raw.replace('.c', '')
  raw = raw.replace('.cxx', '')
  raw = raw.replace('.xx', '')
  raw = raw.replace('.pp', '')
  raw += '.o'
  return raw

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
    cmd.extend(['-c', cxx_src])
    obj_name = prepare_obj_name(cxx_src)
    cmd.extend(['-o', fs.path_abs(f'{ctx.obj_dir}{obj_name}')])
    cmds.append(cmd)

  return cmds

def make_object_list(ctx: Context) -> str:
  '''создать список объектников, который потом подаётся при линковке через @'''

  result = f'{ctx.obj_dir}objects.txt'
  with open(result, 'w', encoding='utf-8') as f:
    for obj in fs.find(f'{ctx.obj_dir}*.o'):
      f.write(f"{fs.path_linux(obj)}\n")
  return f'@{fs.path_abs(result)}'

def compile_multi(tgt: Target, ctx: Context, host: Host):
  '''Многопоточная компиляция без инкрементальной сборки'''
  start = time.perf_counter()

  obj_cmds = prepare_obj_cmd(tgt, ctx)
  print('> компиляция объектных файлов...')
  exec_multi(obj_cmds, ctx.threads, 600.0, ctx.less_info)

  print(f'> линковка \'{to_yellow(tgt.name)}\'')
  cmd = [ctx.compiler_path]
  cmd.extend([f'-D{define}' for define in tgt.defines])
  cmd.extend([f'-{opt}' for opt in tgt.options])
  cmd.extend([f'-I{fs.path_abs(path)}' for path in tgt.include_dirs])
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
