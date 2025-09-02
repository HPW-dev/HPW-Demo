#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from .. import helper

opts = \
  ' -Q enable_omp=1' \
  ' -Q enable_asan=0' \
  ' -Q host=glfw3' \
  ' -Q compiler=gcc' \
  ' -Q use_netplay=0' \
  ' -Q use_tools=1' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
#NUM_THREADS = 8 # юзать при тормозах
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/game/SConscript' + opts)
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/plugin/graphic-effect/cxx/SConscript' + opts)
helper.exec_cmd('build/bin/HPW')
