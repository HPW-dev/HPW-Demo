#!/usr/bin/env python
from platform import architecture
#import sys
from script.helper import copy_license
from enum import Enum

class Terminal_color:
  GREEN = '\033[92m'
  RED = '\033[91m'
  YELOW = '\033[33m'
  DEFAULT = '\033[0m'

Bitness = Enum('Bitness', ['x32', 'x64'])
System = Enum('System', ['windows', 'linux'])
Compiler = Enum('Compiler', ['gcc', 'clang', 'msvc'])
Opt_level = Enum('Opt_level', ['fast', 'debug', 'optimized_debug', 'stable',
  'x86_64_v1', 'x86_64_v4', 'ecomem', 'core2'])
Host = Enum('Host', ['glfw3', 'sdl2', 'asci', 'none'])
Log_mode = Enum('Log_mode', ['detailed', 'debug', 'release'])

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

def yn2s(cond: bool):
  '''enabled/disabled с покрасом строки'''
  _ret = Terminal_color.GREEN + 'enabled' if cond else Terminal_color.RED + 'disabled'
  return _ret + Terminal_color.DEFAULT

def yelow_text(text: str):
  return Terminal_color.YELOW + text + Terminal_color.DEFAULT

def parse_args():
  '''аргументы запуска скрипта конвертятся в управляющие параметры'''
  global bitness
  global system
  global compiler
  global opt_level
  global host
  global log_mode
  global enable_omp
  global enable_asan
  global static_link
  global build_script

  enable_omp = bool(int(ARGUMENTS.get('enable_omp', 1)))
  enable_asan = bool(int(ARGUMENTS.get('enable_asan', 0)))
  build_script = ARGUMENTS.get('script', 'test/graphic/SConscript')
  assert build_script, 'path to build script needed'
  _architecture = architecture()
  bitness = Bitness.x64 if _architecture[0] == '64bit' else Bitness.x32
  system = System.linux if _architecture[1] == 'ELF' else System.windows
  match ARGUMENTS.get('host', 'glfw3').lower():
    case 'glfw3': host = Host.glfw3
    case 'asci': host = Host.asci
    case 'sdl2': host = Host.sdl2
    case 'none': host = Host.none
    case _: host = Host.glfw3
  match ARGUMENTS.get('compiler', 'gcc').lower():
    case 'clang': compiler = Compiler.clang
    case 'gcc': compiler = Compiler.gcc
    case _: compiler = Compiler.gcc
  match ARGUMENTS.get('opt_level', 'stable').lower():
    case 'optimized_debug': opt_level = Opt_level.optimized_debug
    case 'fast': opt_level = Opt_level.fast
    case 'ecomem': opt_level = Opt_level.ecomem
    case 'core2': opt_level = Opt_level.ecomem
    case 'x86_64_v1': opt_level = Opt_level.x86_64_v1
    case 'x86_64_v4': opt_level = Opt_level.x86_64_v4
    case 'debug': opt_level = Opt_level.debug
    case 'stable': opt_level = Opt_level.stable
    case _: opt_level = Opt_level.stable
  match ARGUMENTS.get('log_mode', 'debug').lower():
    case 'detailed': log_mode = Log_mode.detailed
    case 'release': log_mode = Log_mode.release
    case 'debug': log_mode = Log_mode.debug
    case _: log_mode = Log_mode.debug

def print_params():
  '''отображение параметров билда'''
  print(f'Build script: \"{yelow_text(build_script)}\"')
  print(f'Target: {yelow_text(system.name + ' ' + bitness.name)}')
  print(f'Host: {yelow_text(host.name)}')
  print(f'Compiler: {yelow_text(compiler.name)}')
  print(f'Optimization level: {yelow_text(opt_level.name)}')
  print(f'Static link: {yn2s(static_link)}')
  print(f'OpenMP: {yn2s(enable_omp)}');
  print(f'ASAN checks: {yn2s(enable_asan)}')
  print(f'Log mode: {yelow_text(log_mode.name)}')

def accept_params():
  '''применение параметров к опциям билда C++'''
  global cxx_flags
  global cxx_ldflags
  global cxx_defines

  # билтность системы
  cxx_flags.extend(['-m32' if bitness == Bitness.x32 else '-m64'])

  # система
  cxx_defines.extend(['-DWINDOWS' if system == System.windows else '-DLINUX']);

  # хост
  # ... TODO

  # оптимизации
  match opt_level:
    case Opt_level.fast:
      cxx_flags.extend(['-O0', '-g0'])
      cxx_defines.extend({'-DDEBUG'})
    case Opt_level.debug:
      cxx_flags.extend(['-O0', '-g'])
      cxx_defines.extend({'-DDEBUG'})
    case Opt_level.optimized_debug:
      cxx_flags.extend(['-O2', '-g'])
      cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.stable:
      cxx_flags.extend(['-O2', '-flto=auto'])
      cxx_ldflags.extend(['-s'])
      cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.core2:
      cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=core2'])
      cxx_ldflags.extend(['-s'])
      cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.x86_64_v1:
      cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64'])
      cxx_ldflags.extend(['-s'])
      cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.x86_64_v4:
      cxx_flags.extend(['-Ofast', '-flto=auto', '-mtune=generic', '-march=x86-64-v4'])
      cxx_ldflags.extend(['-s'])
      cxx_defines.extend({'-DNDEBUG'})
    case Opt_level.ecomem: raise ValueError('Need implementation for ecomem optimization mode')
    case _: raise ValueError
  #cxx_flags.

  # OpenMP
  if enable_omp:
    cxx_flags.extend(['-fopenmp'])
    cxx_ldflags.extend(['-fopenmp'])

  # ASAN
  if enable_asan:
    _asan_opts = [
      '-fsanitize=leak', '-fsanitize=address',
      '-fsanitize=undefined', '-fsanitize=float-divide-by-zero',
      '-fsanitize=float-cast-overflow', '-fno-sanitize=null', '-fno-sanitize=alignment',
      '-fno-omit-frame-pointer'
    ]
    cxx_flags.extend([_asan_opts])
    cxx_flags.extend([_asan_opts])

  # Log mode
  match log_mode:
    case Log_mode.detailed: cxx_defines.extend({'-DDETAILED_LOG'})
    case Log_mode.release: pass
    case Log_mode.debug: pass

def build():
  '''сборка проекта'''
  pass # TODO

# main section:
parse_args()
print_params()
accept_params()
build()
#print(f'ld_flags: {cxx_ldflags}')  # TODO delme
#print(f'flags: {cxx_flags}')  # TODO delme
#print(f'defines: {cxx_defines}')  # TODO delme

# TODO delme
'''
sanitize = [
  #'-fsanitize=leak', '-fsanitize=address',
  #'-fsanitize=undefined', '-fsanitize=float-divide-by-zero', '-fsanitize=float-cast-overflow', '-fno-sanitize=null', '-fno-sanitize=alignment',
  #'-fsanitize=thread',
  #'-fno-omit-frame-pointer',
  
  #'-pg', '-no-pie',
]
defines = [
  #'-DCLD_DEBUG', # показать сколько коллизий за тик
  #'-DDETAILED_LOG', # вывод доп инфы
  #'-DECOMEM', # экономия памяти для немощных компов
  #'-DSTABLE_REPLAY', # включает проверки для стабильности реплея
]
cpp_flags = [
  #'-std=c++2b', # clang
  '-std=c++23',
  '-Wall', '-Wfatal-errors',
  '-finput-charset=UTF-8', '-fextended-identifiers', # поддержка UTF-8
  sanitize
]
ld_flags = [
  sanitize,
]

# узнать архитектуру машины
arch_info = architecture()
is_64bit = arch_info[0] == '64bit'
is_linux = arch_info[1] == 'ELF'
compiler = ARGUMENTS.get('compiler', 'g++')

if (compiler != 'clang++'):
  ld_flags.append('-shared-libgcc')
else:
  ld_flags.extend(['-static-libgcc', '-lstdc++.dll', '-pthread'])

if bool(ARGUMENTS.get('detailed_log', 0)):
  defines.append('-DDETAILED_LOG')

if is_linux:
  cpp_flags.append('-fdiagnostics-color=always')
  defines.append('-DLINUX')
else:
  defines.append('-DWINDOWS')

is_debug = bool( int( ARGUMENTS.get('debug', 0) ) )
disable_debug_info = bool( int( ARGUMENTS.get('disable_debug_info', 0) ) )
if is_debug:
  defines.append('-DDEBUG')
  cpp_flags.extend([
    '-O0',
    ('-g0' if disable_debug_info else '-ggdb3'),
    ('-m64' if is_64bit else '-m32')
  ])
else: # release
  defines.extend(['-DNDEBUG'])
  ld_flags.extend(['-flto=auto', '-s', '-g0'])
  cpp_flags.extend([
    '-flto=auto',
    '-Ofast',
    '-mtune=generic',
    '-march=x86-64',
    ('-m64' if is_64bit else '-m32')
  ])

host = ARGUMENTS.get('host', 'glfw3')

copy_license()

env = Environment() # for SCons
SConscript(
  build_script, exports=[
    'env',
    'is_linux',
    'is_debug',
    'ld_flags',
    'cpp_flags',
    'defines',
    'is_64bit',
    'compiler',
    'host',
  ],
  must_exist=True
)
'''
