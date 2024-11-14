#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_omp=1' \
  ' -Q enable_asan=0' \
  ' -Q host=glfw3' \
  ' -Q compiler=gcc' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads()
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/game/SConscript' + opts)
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=src/plugin/graphic-effect/cxx/SConscript' + opts)
