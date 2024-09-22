#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_omp=1' \
  ' -Q enable_asan=0' \
  ' -Q host=glfw3' \
  ' -Q compiler=gcc' \
  ' -Q opt_level=stable' \
  ' -Q log_mode=release'
helper.exec_cmd('scons -j15 -Q script=src/game/SConscript' + opts)
helper.exec_cmd('scons -j15 -Q script=src/plugin/graphic-effect/cxx/SConscript' + opts)
helper.rem_all("build/plugin/effect/*.a") # удалить ненужные .a файлы
