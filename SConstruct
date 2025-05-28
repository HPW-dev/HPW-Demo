#!/usr/bin/env python
from os import environ

prog_name = ARGUMENTS.get('PROG_NAME', '')
sources = ARGUMENTS.get('SOURCES', '').split('@')
lib_path = ARGUMENTS.get('LIB_PATH', '').split('@')
used_libs = ARGUMENTS.get('USED_LIBS', '').split('@')
defines = ARGUMENTS.get('DEFINES', '').split('@')
ld_flags = ARGUMENTS.get('LD_FLAGS', '').split('@')
cxx_flags = ARGUMENTS.get('CXX_FLAGS', '').split('@')
inc_path = ARGUMENTS.get('INCLUDE_PATH', '').split('@')
host_system = ARGUMENTS.get('HOST_SYSTEM', '')
assert(prog_name)
assert(sources)
assert(host_system)
print('SCons opts:')
print(f'prog name: {prog_name}')
print(f'sources: {sources}')
print(f'lib path: {lib_path}')
print(f'used libs: {used_libs}')
print(f'defines: {defines}')
print(f'ld flags: {ld_flags}')
print(f'cxx flags: {cxx_flags}')
print(f'inc path: {inc_path}')
print(f'host system: {host_system}')

DefaultEnvironment(ENV=environ.copy())
env = Environment()
# не юзать MSVC
if host_system == 'windows':
  env = Environment(tools = ['mingw'])
else: # linux
  pass # env = Environment(tools = ['gcc'])

env.Program(
  target = prog_name,
  source = sources,
  LIBPATH = lib_path,
  LIBS = used_libs,
  CPPDEFINES = defines,
  CXXFLAGS = cxx_flags,
  LINKFLAGS = ld_flags,
  CPPPATH = inc_path,
)
