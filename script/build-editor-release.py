#!/usr/bin/env python
import helper

script = "tool/editor/SConscript"
is_debug = 0
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd('build/editor')
