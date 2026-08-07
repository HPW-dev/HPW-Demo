from structs.target import *
from structs.context import *
from structs.host import *
from utils.exec import *
from utils.ui import *
from utils import fs


def compile_single(tgt: Target, ctx: Context, host: Host):
  '''однопоточная простая компиляция без инкрементальной сборки'''
  print(f'Сборка \'{to_yellow(tgt.name)}\'...')
  print(f'Исходники:\n  {to_yellow('\n  '.join(tgt.sources))}')

  cmd = [ctx.compiler_path]
  cmd.extend(tgt.defines)
  cmd.extend(tgt.options)
  cmd.extend(tgt.include_dirs)
  cmd.extend(tgt.sources)
  cmd.extend(tgt.lib_dirs)
  cmd.extend(['-o', fs.path_abs(tgt.name)])
  cmd.extend(tgt.linked_libs)
  print(f'Команда компиляции:\n  {to_yellow(' '.join(cmd))}')
  exec_cmd(cmd, timeout=60.0*30.0)

  if not fs.exists(tgt.name):
    raise FileExistsError(f'не удалось создать файл \'{tgt.name}\'')
