#!/usr/bin/env python
import platform
import sys
from script.helper import copy_license

sanitize = [
  #"-fsanitize=leak", "-fsanitize=address",
  #"-fsanitize=undefined", "-fsanitize=float-divide-by-zero", "-fsanitize=float-cast-overflow", "-fno-sanitize=null", "-fno-sanitize=alignment",
  #"-fsanitize=thread",
  #"-fno-omit-frame-pointer",
  
  #"-pg", "-no-pie",
]
pgo = [
  #"-fprofile-dir=pgo", "-fprofile-generate=pgo",
  #"-fprofile-dir=pgo", "-fprofile-use=pgo", "-fprofile-correction",
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
  sanitize,
  pgo
]
ld_flags = [
  sanitize,
  pgo
]

# узнать архитектуру машины
arch_info = platform.architecture()
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

script = ARGUMENTS.get("script", "test/graphic/SConscript")
host = ARGUMENTS.get("host", "glfw3")

print("Execute script: " + script)
print("System: " +
  ("Linux" if is_linux else "Windows") + " " +
  ("x64" if is_64bit else "x32"))
print(f"Host: {host}")
print("Build mode: " + ("debug" if is_debug else "release"))
print("Compiler: " + compiler)

copy_license()

env = Environment() # for SCons
SConscript(
  script, exports=[
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
