#!/usr/bin/env python
import helper

script = "tool/editor/SConscript"
is_debug = 1
disable_debug_info = 0
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q disable_debug_info={disable_debug_info} -Q script={script}')
helper.exec_cmd('build/bin/editor')
