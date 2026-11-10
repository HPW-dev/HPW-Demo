from actions.prepare_game_ver import *
from actions.compile import *
from structs.target import *
from structs.context import *
from structs.host import *
from utils.ui import *
from utils.fs import *

def exclude_files(src: list[str], ban_list: list[str]):
  '''
  Убирает из списка файлов для сборки ненужные файлы из ban_list
  '''
  to_remove_set = {path_abs(path) for path in ban_list}
  return [path for path in src if path_abs(path) not in to_remove_set]

def compile_game(tgt: Target, ctx: Context, host: Host):
  print('=== Сборка игры ===')

  executable = path_abs(ctx.bin_dir + tgt.name)
  thirdparty_dir = "thirdparty/"
  src_dir = ctx.src_dir
  bits = host.bitness

  if tgt.use_openmp:
    tgt.options.append('fopenmp')

  if host.system == Sys_name.windows:
    tgt.linked_libs.extend([
      '-lglfw3dll',
      '-lopengl32',
      '-lOpenAL32.dll',
    ])
    # можно и консоль вырубить, если это не релиз:
    if tgt.opt_preset != 'debug-x32' and \
       tgt.opt_preset != 'debug-x64' and \
       tgt.opt_preset != 'fast-build':
      print(to_green('> Вырубаем консоль для H.P.W'))
      tgt.linked_libs.append('-mwindows')
  elif host.system == Sys_name.linux:
    tgt.linked_libs.extend([
      '-lglfw',
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
      f'{thirdparty_dir}lib/OpenAL-soft/{bits}',
      f'{thirdparty_dir}lib/GLFW/{bits}',
    ])
  elif host.system == Sys_name.linux:
    pass

  tgt.defines.append('WINDOWS' if host.system == Sys_name.windows else 'LINUX')
  tgt.defines.append('PORT_GLFW3') # TODO остальные хосты
  if ctx.with_tests:
    tgt.defines.append('USE_TESTS')
  if not ctx.with_gameplay:
    tgt.defines.append('NO_GAMEPLAY')

  netplay_sources = []
  if ctx.with_lan:
    tgt.defines.extend(['USE_LAN', 'ASIO_STANDALONE'])
    netplay_sources.extend(find(f'{src_dir}game/netplay/scene/*.cpp'))

  tgt.include_dirs.extend([
    '.',
    src_dir,
    f'{thirdparty_dir}include/',
    f'{thirdparty_dir}include/glad2/include/',
  ])
  if host.system == Sys_name.windows:
    tgt.include_dirs.extend([
      f'{thirdparty_dir}include/_windows_only/GLFW/{bits}',
    ])

  tgt.sources.append(f'{thirdparty_dir}include/glad2/src/gl.c')
  tgt.sources.extend(find(f'{thirdparty_dir}include/zip/*.c'))
  
  tgt.sources.append(f'{thirdparty_dir}include/stb/stb_vorbis.c')
  tgt.sources.append(f'{thirdparty_dir}include/Nayuki/QR-Code-generator/qrcodegen.cpp')

  tgt.sources.extend(find(f'{src_dir}util/file/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}util/*.c'))
  tgt.sources.extend(find(f'{src_dir}util/*.cpp'))

  tgt.sources.extend(find(f'{src_dir}port/windows/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}port/glfw3/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}port/ogl3/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}port/*.cpp'))

  # версию надо генерить перед game/util/*, иначе find не найдёт нужное
  generate_game_version_file(ctx, tgt)

  tgt.sources.extend(find(f'{src_dir}game/util/post-effect/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/util/cmd/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/util/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/input/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/bgp/legacy/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/bgp/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/core/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/menu/item/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/menu/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/options/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/common/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/msgbox/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/scene/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}game/*.cpp'))

  if ctx.with_gameplay:
    tgt.sources.extend(find(f'{src_dir}engine/graphic/animation/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/player/ability/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/player/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/enemy/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/collider/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/util/info/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/util/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/entity/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/scene/gameplay/cutscene/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/scene/gameplay/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/level/util/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/level/*.cpp'))
    tgt.sources.extend(find(f'{src_dir}game/hud/*.cpp'))
  else:
    print('> сборка без уровней и геймплея...')
    tgt.sources = exclude_files(tgt.sources, [
      f'{src_dir}game/scene/options/scene-hud-select.cpp',
      f'{src_dir}game/util/difficulty-helper.cpp',
      f'{src_dir}game/util/animation-helper.cpp',
      f'{src_dir}game/util/anim-helper.cpp',
      f'{src_dir}game/util/camera.cpp',
      f'{src_dir}game/util/shop.cpp',
      f'{src_dir}game/util/cmd/cmd-entity.cpp',
      f'{src_dir}game/util/cmd/cmd-player.cpp',
      f'{src_dir}game/util/cmd/cmd-level.cpp',
    ])

  if ctx.with_tests:
    tgt.sources.extend(find(f'{src_dir}game/scene/tests/*.cpp'))
  
  tgt.sources.extend(find(f'{src_dir}engine/graphic/epge/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/image/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/sprite/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/effect/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/font/*.cpp'))
  tgt.sources.extend(find(f'{src_dir}engine/graphic/util/*.cpp'))

  tgt.sources.extend(find(f'{src_dir}engine/sound/*.cpp'))

  tgt.sources += netplay_sources

  tgt.sources.extend(find(f'{src_dir}*.cpp'))

  if ctx.with_game_ver:
    tgt.game_ver_file = 'builder/actions/compile/version.rc'

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
