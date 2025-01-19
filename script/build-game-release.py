#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_omp=1' \
  ' -Q enable_asan=0' \
  ' -Q host=glfw3' \
  ' -Q compiler=gcc' \
  ' -Q use_netplay=1' \
  ' -Q opt_level=stable'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/game/SConscript' + opts)
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/plugin/graphic-effect/cxx/SConscript' + opts)
helper.rem_all("build/plugin/effect/*.a") # удалить ненужные .a файлы
helper.exec_cmd('build/bin/HPW')
