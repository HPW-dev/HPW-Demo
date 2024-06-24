#!/usr/bin/env python
import helper

script = "tool/code-to-image/SConscript"
is_debug = 1
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd('build/code-to-image .')
