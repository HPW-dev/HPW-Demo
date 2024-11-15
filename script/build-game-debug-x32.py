#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_omp=1' \
  ' -Q enable_asan=0' \
  ' -Q host=glfw3' \
  ' -Q compiler=gcc' \
  ' -Q opt_level=debug' \
  ' -Q bitness=x32'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/game/SConscript' + opts)
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/plugin/graphic-effect/cxx/SConscript' + opts)
helper.exec_cmd('build/bin/HPW')
