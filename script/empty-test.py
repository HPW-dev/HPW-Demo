#!/usr/bin/env python
import helper

script = "test/empty/SConscript"
is_debug = 1
disable_debug_info = 1
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} '
  f'-Q disable_debug_info={disable_debug_info} -Q script={script}')
helper.exec_cmd('build/empty-test')