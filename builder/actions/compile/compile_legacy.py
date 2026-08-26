from actions.prepare_game_ver import *
from actions.compile import *
from structs.target import *
from structs.context import *
from structs.host import *
from utils.ui import *
from utils.fs import *

def compile_legacy(tgt: Target, ctx: Context, host: Host):
  print('=== Сборка старой версии игры ===')

  executable = path_abs(ctx.bin_dir + tgt.name)
  thirdparty_dir = "thirdparty/"
  src_dir = ctx.src_dir
  bits = host.bitness

  if tgt.use_openmp:
    tgt.options.append('fopenmp')

  if host.system == Sys_name.windows:
    tgt.linked_libs.extend([
      '-lyaml-cpp',
      '-lglfw3dll',
      '-lglew32',
      '-lopengl32',
      '-ldl',
      '-lOpenAL32.dll',
    ])
  elif Sys_name.linux:
    tgt.linked_libs.extend([
      '-lyaml-cpp',
      '-lglfw',
      '-lGLEW',
      '-lGL',
      '-lm',
      '-lopenal',
      '-fpie',
    ])
  tgt.linked_libs.extend([
    '-shared-libgcc',
  ])

  if host.system == Sys_name.windows:
    tgt.lib_dirs.extend([
      f'{thirdparty_dir}lib/yaml-cpp/{bits}',
      f'{thirdparty_dir}lib/OpenAL-soft/{bits}',
      f'{thirdparty_dir}lib/GLEW/{bits}',
      f'{thirdparty_dir}lib/GLFW/{bits}',
    ])
  elif host.system == Sys_name.linux:
    pass

  tgt.defines.append('WINDOWS' if host.system == Sys_name.windows else 'LINUX')
  tgt.defines.append('HOST_GLFW3') # TODO остальные хосты

  tgt.include_dirs.extend([
    '.',
    src_dir,
    f'{thirdparty_dir}include/',
  ])
  if host.system == Sys_name.windows:
    tgt.include_dirs.extend([
      f'{thirdparty_dir}include/_windows_only/GLFW/{bits}',
      f'{thirdparty_dir}include/_windows_only/GLEW/',  
    ])

  tgt.sources.extend(find(f'{thirdparty_dir}include/zip/*.c'))
  tgt.sources.append(f'{thirdparty_dir}include/stb/stb_vorbis.c')

  tgt.sources.extend(find(f'{src_dir}util/file/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}util/math/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}util/math/*.c'))
  tgt.sources.extend(find(f'{src_dir}util/*.cpp'))

  tgt.sources.extend(find(f'{src_dir}host/windows/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}host/glfw3/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}host/ogl3/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}host/*.cpp'))

  # версию надо генерить перед game/util/*, иначе find не найдёт нужное
  generate_game_version_file(ctx)

  tgt.sources.extend(find(f'{src_dir}game/util/post-effect/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/util/cmd/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/util/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/bgp/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/bgp/legacy/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/core/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/hud/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/player/ability/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/player/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/enemy/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/collider/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/util/info/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/util/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/entity/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/menu/item/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/menu/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/cutscene/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/msgbox/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/level/util/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/level/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/*.cpp'))
  
  tgt.sources.extend(find(f'{src_dir}engine/graphic/epge/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/animation/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/image/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/sprite/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/effect/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/font/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/util/*.cpp'))

  tgt.sources.extend(find(f'{src_dir}engine/sound/*.cpp'))

  tgt.sources.extend(find(f'{src_dir}*.cpp'))

  if ctx.with_incremental:
    print('используется инкрементальная многопоточная сборка')
    compile_multi_incremental(tgt, ctx, host)
  else:
    print('используется многопоточная сборка')
    compile_multi(tgt, ctx, host)

  if exists(executable):
    print(to_green(f'Файл для запуска игры "{executable}" готов'))
  else:
    raise FileNotFoundError(f'Не удалось создать экзешник "{executable}"')
