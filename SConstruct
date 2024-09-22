#!/usr/bin/env python
from platform import architecture
#import sys
from script.helper import copy_license
from enum import Enum

Bitness = Enum('Bitness', ['x32', 'x64'])
System = Enum('System', ['windows', 'linux'])
Compiler = Enum('Compiler', ['gcc', 'clang', 'msvc'])
Opt_level = Enum('Opt_level', ['fast', 'debug', 'optimized_debug', 'stable', 'x86_64_v1', 'x86_64_v4', 'ecomem'])
Host = Enum('Host', ['glfw3', 'sdl2', 'asci', 'none'])
Link_mode = Enum('Link_mode', ['static', 'shared'])
Log_mode = Enum('Log_mode', ['detailed', 'debug', 'release'])

# управляющие переменные:

bitness = Bitness.x64
system = System.windows
compiler = Compiler.gcc
opt_level = Opt_level.debug
host = Host.glfw3
link_mode = Link_mode.shared
log_mode = Log_mode.debug
enable_omp = False
enable_asan = False
build_script = ""

# парс параметров
def parse_args():
  enable_omp = bool(int(ARGUMENTS.get("enable_omp", 0)))
  enable_asan = bool(int(ARGUMENTS.get("enable_asan", 0)))
  build_script = ARGUMENTS.get("script", "test/graphic/SConscript")
  assert build_script, "path to build script needed"
  _architecture = architecture()
  bitness = Bitness.x64 if _architecture[0] == "64bit" else Bitness.x32
  system = System.linux if _architecture[1] == "ELF" else System.windows
  match ARGUMENTS.get("host", "glfw3"):
    case "glfw3": host = Host.glfw3
    case "asci": host = Host.asci
    case "sdl2": host = Host.sdl2
    case "none": host = Host.none
    case _: host = Host.glfw3

# отображение параметров билда
def print_params():
  print(f"Optimization level: {opt_level.name}")
  print(f"Build script: \"{build_script}\"")
  print(f"ASAN checks: {"enabled" if enable_asan else "disabled"}")
  print(f"Link mode: {link_mode.name}")
  print(f"Log mode: {log_mode.name}")
  print(f"Compiler: {compiler.name}")
  print(f"OpenMP: {"enabled" if enable_omp else "disabled"}");
  print(f"Target: {system.name} {bitness.name}")
  print(f"Host: {host.name}")

# main section:
parse_args()
print_params()




'''
sanitize = [
  #"-fsanitize=leak", "-fsanitize=address",
  #"-fsanitize=undefined", "-fsanitize=float-divide-by-zero", "-fsanitize=float-cast-overflow", "-fno-sanitize=null", "-fno-sanitize=alignment",
  #"-fsanitize=thread",
  #"-fno-omit-frame-pointer",
  
  #"-pg", "-no-pie",
]
defines = [
  #"-DCLD_DEBUG", # показать сколько коллизий за тик
  #"-DDETAILED_LOG", # вывод доп инфы
  #"-DECOMEM", # экономия памяти для немощных компов
  #"-DSTABLE_REPLAY", # включает проверки для стабильности реплея
]
cpp_flags = [
  #"-std=c++2b", # clang
  "-std=c++23",
  "-Wall", "-Wfatal-errors",
  "-finput-charset=UTF-8", "-fextended-identifiers", # поддержка UTF-8
  sanitize
]
ld_flags = [
  sanitize,
]

# узнать архитектуру машины
arch_info = architecture()
is_64bit = arch_info[0] == "64bit"
is_linux = arch_info[1] == "ELF"
compiler = ARGUMENTS.get("compiler", "g++")

if (compiler != "clang++"):
  ld_flags.append("-shared-libgcc")
else:
  ld_flags.extend(["-static-libgcc", "-lstdc++.dll", "-pthread"])

if bool(ARGUMENTS.get("detailed_log", 0)):
  defines.append("-DDETAILED_LOG")

if is_linux:
  cpp_flags.append("-fdiagnostics-color=always")
  defines.append("-DLINUX")
else:
  defines.append("-DWINDOWS")

is_debug = bool( int( ARGUMENTS.get("debug", 0) ) )
disable_debug_info = bool( int( ARGUMENTS.get("disable_debug_info", 0) ) )
if is_debug:
  defines.append("-DDEBUG")
  cpp_flags.extend([
    "-O0",
    ("-g0" if disable_debug_info else "-ggdb3"),
    ("-m64" if is_64bit else "-m32")
  ])
else: # release
  defines.extend(["-DNDEBUG"])
  ld_flags.extend(["-flto=auto", "-s", "-g0"])
  cpp_flags.extend([
    "-flto=auto",
    "-Ofast",
    "-mtune=generic",
    "-march=x86-64",
    ("-m64" if is_64bit else "-m32")
  ])

host = ARGUMENTS.get("host", "glfw3")

copy_license()

env = Environment() # for SCons
SConscript(
  build_script, exports=[
    "env",
    "is_linux",
    "is_debug",
    "ld_flags",
    "cpp_flags",
    "defines",
    "is_64bit",
    "compiler",
    "host",
  ],
  must_exist=True
)
'''
