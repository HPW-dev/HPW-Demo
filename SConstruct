#!/usr/bin/env python
from platform import architecture
#import sys
from os import environ
from script.helper import *

class Terminal_color:
  GREEN = '\033[92m'
  RED = '\033[91m'
  YELOW = '\033[33m'
  DEFAULT = '\033[0m'

class Hpw_config:
  # управляющие переменные:
  bitness = Bitness.x64
  system = System.windows
  compiler = Compiler.gcc
  opt_level = Opt_level.debug
  host = Host.glfw3
  log_mode = Log_mode.debug
  enable_omp = True
  enable_asan = False
  static_link = False
  build_script = ''

  # опции билда C++:
  cxx_defines = []
  cxx_ldflags = []
  cxx_flags = []

hpw_config = Hpw_config()

def yn2s(cond: bool):
  '''enabled/disabled с покрасом строки'''
  _ret = Terminal_color.GREEN + 'enabled' if cond else Terminal_color.RED + 'disabled'
  return _ret + Terminal_color.DEFAULT

def yelow_text(text: str):
  return Terminal_color.YELOW + text + Terminal_color.DEFAULT

def parse_args():
  '''аргументы запуска скрипта конвертятся в управляющие параметры'''
  global hpw_config

  hpw_config.enable_omp = bool(int(ARGUMENTS.get('enable_omp', 1)))
  hpw_config.enable_asan = bool(int(ARGUMENTS.get('enable_asan', 0)))
  hpw_config.build_script = ARGUMENTS.get('script', 'test/graphic/SConscript')
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
    case 'core2': hpw_config.opt_level = Opt_level.ecomem
    case 'x86_64_v1': hpw_config.opt_level = Opt_level.x86_64_v1
    case 'x86_64_v4': hpw_config.opt_level = Opt_level.x86_64_v4
    case 'debug': hpw_config.opt_level = Opt_level.debug
    case 'stable': hpw_config.opt_level = Opt_level.stable
    case _: hpw_config.opt_level = Opt_level.stable
  match ARGUMENTS.get('log_mode', 'debug').lower():
    case 'detailed': hpw_config.log_mode = Log_mode.detailed
    case 'release': hpw_config.log_mode = Log_mode.release
    case 'debug': hpw_config.log_mode = Log_mode.debug
    case _: hpw_config.log_mode = Log_mode.debug

def print_params():
  '''отображение параметров билда'''
  print(f'Build script: \"{yelow_text(hpw_config.build_script)}\"')
  print(f'Target: {yelow_text(hpw_config.system.name + ' ' + hpw_config.bitness.name)}')
  print(f'Host: {yelow_text(hpw_config.host.name)}')
  print(f'Compiler: {yelow_text(hpw_config.compiler.name)}')
  print(f'Optimization level: {yelow_text(hpw_config.opt_level.name)}')
  print(f'Static link: {yn2s(hpw_config.static_link)}')
  print(f'OpenMP: {yn2s(hpw_config.enable_omp)}');
  print(f'ASAN checks: {yn2s(hpw_config.enable_asan)}')
  print(f'Log mode: {yelow_text(hpw_config.log_mode.name)}')

def accept_params():
  '''применение параметров к опциям билда C++'''
  global hpw_config

  # общие параметры:
  hpw_config.cxx_flags.extend([
    '-std=c++23',
    '-Wall', '-Wfatal-errors', # останавливать компиляцию при первой ошибке
    '-finput-charset=UTF-8', '-fextended-identifiers', # поддержка UTF-8
  ])

  # билтность системы
  hpw_config.cxx_flags.extend(['-m32' if hpw_config.bitness == Bitness.x32 else '-m64'])

  # система
  hpw_config. cxx_defines.extend(['-DWINDOWS' if hpw_config.system == System.windows else '-DLINUX'])
  if hpw_config.system == System.linux:
    cxx_flags.extend(['-fdiagnostics-color=always'])

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
      hpw_config.cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.stable:
      hpw_config.cxx_flags.extend(['-O2', '-flto=auto'])
      hpw_config.cxx_ldflags.extend(['-s', '-mwindows'])
      hpw_config.cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.core2:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=core2'])
      hpw_config.cxx_ldflags.extend(['-s', '-mwindows'])
      hpw_config.cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.x86_64_v1:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64'])
      hpw_config.cxx_ldflags.extend(['-s', '-mwindows'])
      hpw_config.cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.x86_64_v4:
      hpw_config.cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64-v4'])
      hpw_config.cxx_ldflags.extend(['-s', '-mwindows'])
      hpw_config.cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.ecomem: raise ValueError('Need implementation for ecomem optimization mode')
    case _: raise ValueError
  #cxx_flags.

  # OpenMP
  if hpw_config.enable_omp:
    hpw_config.cxx_flags.extend(['-fopenmp'])
    hpw_config.cxx_ldflags.extend(['-fopenmp'])

  # ASAN
  if hpw_config.enable_asan:
    _asan_opts = [
      '-fsanitize=leak', '-fsanitize=address',
      '-fsanitize=undefined', '-fsanitize=float-divide-by-zero',
      '-fsanitize=float-cast-overflow', '-fno-sanitize=null', '-fno-sanitize=alignment',
      '-fno-omit-frame-pointer'
    ]
    hpw_config.cxx_flags.extend([_asan_opts])
    hpw_config.cxx_flags.extend([_asan_opts])

  # Log mode
  match hpw_config.log_mode:
    case Log_mode.detailed: hpw_config.cxx_defines.extend({'-DDETAILED_LOG'})
    case Log_mode.release: pass
    case Log_mode.debug: pass

  if hpw_config.static_link:
    hpw_config.cxx_ldflags.extend(['-static-libgcc'])
  else:
    hpw_config.cxx_ldflags.extend(['-shared-libgcc'])

def build():
  '''сборка проекта'''
  env = Environment(ENV=environ.copy())
  # скопировать нужные переменные для экспорта
  env['hpw_config'] = hpw_config
  SConscript(hpw_config.build_script, exports=['env'], must_exist=True)

# main section:
parse_args()
print_params()
accept_params()
copy_license()
write_game_version()
build()
