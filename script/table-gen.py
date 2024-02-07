#!/usr/bin/env python
import helper

script = "tool/table-gen/SConscript"
is_debug = 0
helper.exec_cmd(f'scons -j4 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd("build/table gen")
