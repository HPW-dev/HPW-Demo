from actions.exec_multi import *
from structs.target import *
from structs.context import *
from structs.host import *
from utils.exec import exec_cmd
from utils.ui import *
from utils import fs


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
    cmd.extend(tgt.defines)
    cmd.extend(tgt.options)
    cmd.extend(tgt.include_dirs)
    cmd.extend(['-c', cxx_src])
    cmd.extend(tgt.lib_dirs)
    obj_name = prepare_obj_name(cxx_src)
    cmd.extend(['-o', fs.path_abs(f'{ctx.obj_dir}{obj_name}')])
    cmd.extend(tgt.linked_libs)
    cmds.append(cmd)

  return cmds

def compile_multi(tgt: Target, ctx: Context, host: Host):
  '''Многопоточная компиляция без инкрементальной сборки'''

  obj_cmds = prepare_obj_cmd(tgt, ctx)
  print('> компиляция объектных файлов...')
  exec_multi(obj_cmds, ctx.threads, 600.0)

  print(f'> линковка \'{to_yellow(tgt.name)}\'')
  cmd = [ctx.compiler_path]
  cmd.extend(tgt.defines)
  cmd.extend(tgt.options)
  cmd.extend(tgt.include_dirs)
  cmd.append(f'{ctx.obj_dir}*.o')
  cmd.extend(tgt.lib_dirs)
  cmd.extend(['-o', fs.path_abs(tgt.name)])
  cmd.extend(tgt.linked_libs)
  print(to_yellow(' '.join(cmd)))
  exec_cmd(cmd, 600.0)

  if not fs.exists(tgt.name):
    raise FileExistsError(f'не удалось создать файл \'{tgt.name}\'')
