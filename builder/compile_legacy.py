from actions.compile import *
from structs.target import *
from structs.context import *
from structs.host import *
from utils.ui import *
from utils.fs import *

def compile_legacy(tgt: Target, ctx: Context, host: Host):
  print(to_gray('Сборка старой версии игры...'))

  executable = path_abs(ctx.bin_dir + tgt.name)
  thirdparty_dir = "thirdparty/"
  src_dir = "src/"
  bits = host.bitness

  tgt.options.extend([
    'Wall', 'std=c++26', 'pipe',
    's', 'Ofast', 'march=x86-64', 'mtune=generic'
  ])
  tgt.linked_libs.extend([
    '-lyaml-cpp',
    '-lglfw3dll',
    '-lglew32',
    '-lopengl32',
    '-lOpenAL32.dll',
    '-static-libgcc',
  ])
  tgt.lib_dirs.extend([
    f'{thirdparty_dir}lib/yaml-cpp/{bits}',
    f'{thirdparty_dir}lib/OpenAL-soft/{bits}',
    f'{thirdparty_dir}lib/GLEW/{bits}',
    f'{thirdparty_dir}lib/GLFW/{bits}',
  ])
  if tgt.use_openmp:
    tgt.linked_libs.append('-fopenmp')
  tgt.defines.extend([
    'HOST_GLFW3',
    'WINDOWS',
  ])
  tgt.include_dirs.extend([
    '.',
    src_dir,
    f'{thirdparty_dir}include/',
    f'{thirdparty_dir}include/_windows_only/GLFW/{bits}',
  ])
  tgt.sources = find(f'{src_dir}/*.cpp')

  compile_multi(tgt, ctx, host)

  if exists(executable):
    print(to_green(f'Файл для запуска игры: {executable}'))
  else:
    raise RuntimeError(f'Не улаорсь создать экзешник "{executable}"')
