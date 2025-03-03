#!/usr/bin/env python
from script.helper import *
from script.diff_checker import compress_diffs
print(f'python version: {check_python_version()}')

from platform import architecture
#import sys
from os import environ

class Terminal_color:
  GREEN = '\033[92m'
  RED = '\033[91m'
  YELOW = '\033[33m'
  DEFAULT = '\033[0m'

hpw_config = Hpw_config()

def yn2s(cond: bool):
  '''enabled/disabled с покрасом строки'''
  _ret = Terminal_color.GREEN + 'enabled' if cond else Terminal_color.RED + 'disabled'
  return _ret + Terminal_color.DEFAULT

def yelow_text(text: str):
  return Terminal_color.YELOW + text + Terminal_color.DEFAULT

def green_text(text: str):
  return Terminal_color.GREEN + text + Terminal_color.DEFAULT

def parse_args():
  '''аргументы запуска скрипта конвертятся в управляющие параметры'''
  global hpw_config

  hpw_config.custom_cxx = ARGUMENTS.get('cxx', '')
  hpw_config.custom_cc = ARGUMENTS.get('cc', '')
  hpw_config.enable_omp = bool(int(ARGUMENTS.get('enable_omp', 1)))
  hpw_config.enable_asan = bool(int(ARGUMENTS.get('enable_asan', 0)))
  hpw_config.build_script = ARGUMENTS.get('script', 'test/graphic/SConscript')
  hpw_config.use_data_zip = bool(int(ARGUMENTS.get('use_data_zip', 1)))
  hpw_config.use_netplay = bool(int(ARGUMENTS.get('use_netplay', 0)))
  hpw_config.disable_graphic = bool(int(ARGUMENTS.get('disable_graphic', 0)))
  hpw_config.use_ccache = bool(int(ARGUMENTS.get('use_ccache', 0)))
  assert hpw_config.build_script, 'path to build script needed'
  _architecture = architecture()
  hpw_config.system = System.linux if _architecture[1] == 'ELF' else System.windows
  match ARGUMENTS.get('bitness', 'auto').lower():
    case 'x32': hpw_config.bitness = Bitness.x32
    case 'x64': hpw_config.bitness = Bitness.x64
    case 'auto': hpw_config.bitness = Bitness.x64 if _architecture[0] == '64bit' else Bitness.x32
  match ARGUMENTS.get('host', 'glfw3').lower():
    case 'glfw3': hpw_config.host = Host.glfw3
    case 'asci': hpw_config.host = Host.asci
    case 'sdl2': hpw_config.host = Host.sdl2
    case 'none': hpw_config.host = Host.none
    case _: hpw_config.host = Host.glfw3
  match ARGUMENTS.get('compiler', 'gcc').lower():
    case 'clang': hpw_config.compiler = Compiler.clang
    case 'gcc': hpw_config.compiler = Compiler.gcc
    case _: hpw_config.compiler = Compiler.gcc
  match ARGUMENTS.get('opt_level', 'stable').lower():
    case 'optimized_debug': hpw_config.opt_level = Opt_level.optimized_debug
    case 'fast': hpw_config.opt_level = Opt_level.fast
    case 'ecomem': hpw_config.opt_level = Opt_level.ecomem
    case 'core2': hpw_config.opt_level = Opt_level.core2
    case 'atom': hpw_config.opt_level = Opt_level.atom
    case 'x86_64_v1': hpw_config.opt_level = Opt_level.x86_64_v1
    case 'x86_64_v4': hpw_config.opt_level = Opt_level.x86_64_v4
    case 'debug': hpw_config.opt_level = Opt_level.debug
    case 'stable': hpw_config.opt_level = Opt_level.stable
    case 'i386_stable': hpw_config.opt_level = Opt_level.i386_stable
    case 'i386': hpw_config.opt_level = Opt_level.i386
    case _: hpw_config.opt_level = Opt_level.stable

def print_params():
  '''отображение параметров билда'''
  print(f'Build script: \"{yelow_text(hpw_config.build_script)}\"')
  print(f'Target OS: {yelow_text(hpw_config.system.name + ' ' + hpw_config.bitness.name)}')
  print(f'Host: {yelow_text(hpw_config.host.name)}')
  print(f'Compiler: {yelow_text(hpw_config.compiler.name)}')
  print(f'Optimization level: {yelow_text(hpw_config.opt_level.name)}')
  print(f'Static link: {yn2s(hpw_config.static_link)}')
  print(f'OpenMP: {yn2s(hpw_config.enable_omp)}');
  print(f'Netplay: {yn2s(hpw_config.use_netplay)}')
  print(f'Graphic: {yn2s(not hpw_config.disable_graphic)}')
  print(f'ASAN checks: {yn2s(hpw_config.enable_asan)}')
  print(f'CXX: {green_text(hpw_config.custom_cxx) if hpw_config.custom_cxx else yelow_text('default')}')
  print(f'CC: {green_text(hpw_config.custom_cc) if hpw_config.custom_cc else yelow_text('default')}')

def accept_params():
  '''применение параметров к опциям билда C++'''
  global hpw_config

  # общие параметры:
  hpw_config.cxx_flags.extend([
    '-std=c++23',
    '-Wall', '-Wfatal-errors', # останавливать компиляцию при первой ошибке
    # '-Wextra', '-pedantic', '-Wno-unused-parameter', # больше ворнингов!
    '-finput-charset=UTF-8', '-fextended-identifiers', # поддержка UTF-8
  ])

  # разрядность системы
  hpw_config.cxx_flags.extend(['-m32' if hpw_config.bitness == Bitness.x32 else '-m64'])

  # система
  hpw_config.cxx_defines.extend(['-DWINDOWS' if hpw_config.system == System.windows else '-DLINUX'])
  if hpw_config.system == System.linux:
    hpw_config.cxx_flags.extend(['-fdiagnostics-color=always'])

  # хост
  match hpw_config.host:
    case Host.glfw3: hpw_config.cxx_defines.extend(["-DHOST_GLFW3"])
    case Host.asci: hpw_config.cxx_defines.extend(["-DHOST_ASCI"])
    case Host.sdl2: ValueError('need impl. for SDL2 host')
    case Host.none: ValueError('need impl. for none-host')

  # оптимизации
  match hpw_config.opt_level:
    case Opt_level.fast:
      hpw_config.cxx_flags.extend(['-O0', '-g0'])
      hpw_config.cxx_defines.extend({'-DDEBUG'})
      
    case Opt_level.debug:
      hpw_config.cxx_flags.extend(['-O0', '-g'])
      hpw_config.cxx_defines.extend({'-DDEBUG'})

    case Opt_level.optimized_debug:
      hpw_config.cxx_flags.extend(['-O2', '-g'])
      hpw_config.cxx_defines.extend({'-DDEBUG'})

    case Opt_level.stable:
      hpw_config.cxx_flags.extend(['-O2', '-flto=auto'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})

    case Opt_level.core2:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=core2', '-march=core2'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})

    case Opt_level.atom:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=atom', '-march=atom'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})

    case Opt_level.x86_64_v1:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})

    case Opt_level.x86_64_v4:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64-v4'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})
    
    case Opt_level.i386_stable:
      if hpw_config.bitness != Bitness.x32:
        raise ValueError('Need x32 build options for i386 CPU')
      hpw_config.cxx_flags.extend(['-O2', '-flto=auto', '-mtune=i386', '-march=i386'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})
    
    case Opt_level.i386:
      if hpw_config.bitness != Bitness.x32:
        raise ValueError('Need x32 build options for i386 CPU')
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=i386', '-march=i386'])
      if hpw_config.system == System.windows:
        hpw_config.cxx_ldflags.extend(['-mwindows'])
      hpw_config.cxx_ldflags.extend(['-s'])
      hpw_config.cxx_defines.extend({'-DNDEBUG', '-DRELEASE'})

    case Opt_level.ecomem: raise ValueError('Need implementation for ecomem optimization mode')

    case _: raise ValueError

  # OpenMP
  if hpw_config.enable_omp:
    hpw_config.cxx_flags.extend(['-fopenmp'])
    hpw_config.cxx_ldflags.extend(['-fopenmp'])

  # сетевая игра
  if hpw_config.use_netplay:
    hpw_config.cxx_defines.append('-DUSE_NETPLAY')
  
  # режим без графики
  if hpw_config.disable_graphic:
    hpw_config.cxx_defines.append('-DDISABLE_GRAPHIC')

  # ASAN
  if hpw_config.enable_asan:
    _asan_opts = [
      '-fsanitize=leak', '-fsanitize=address',
      '-fsanitize=undefined', '-fsanitize=float-divide-by-zero',
      '-fsanitize=float-cast-overflow', '-fno-sanitize=null', '-fno-sanitize=alignment',
      '-fno-omit-frame-pointer'
    ]
    hpw_config.cxx_flags.extend([_asan_opts])
    hpw_config.cxx_ldflags.extend([_asan_opts])

  if hpw_config.static_link:
    hpw_config.cxx_ldflags.extend(['-static-libgcc'])
  else:
    hpw_config.cxx_ldflags.extend(['-shared-libgcc'])

def build():
  '''сборка проекта'''
  env = Environment(ENV=environ.copy())
  # не юзать MSVC
  if hpw_config.system == System.windows:
    env = Environment(tools = ['mingw'])
  else:
    pass # env = Environment(tools = ['gcc'])

  # скопировать нужные переменные для экспорта
  env['hpw_config'] = hpw_config
  if hpw_config.custom_cxx:
    env['CXX'] = hpw_config.custom_cxx
  if hpw_config.custom_cc:
    env['CC'] = hpw_config.custom_cc
  if hpw_config.use_ccache:
    ccache_str = "ccache -o LOCALAPPDATA=.tmp "
    env['CXX'] = ccache_str + env['CXX']
    env['CC'] = ccache_str + env['CC']
  
  # выключение использование архива с ресурсами
  if not hpw_config.use_data_zip:
    print('generation data.zip: disabled')
    hpw_config.cxx_defines.append("-DDISABLE_ARCHIVE")

  SConscript(hpw_config.build_script, exports=['env'], must_exist=True)

# main section:
parse_args()
print_params()
accept_params()
copy_license()
write_game_version()
build()

if hpw_config.use_data_zip:
  compress_diffs('./data/', './build/data', './.tmp/resources_diff.pickle')
