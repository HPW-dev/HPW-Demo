#!/usr/bin/env python
import helper

script = "test/graphic/SConscript"
#compiler = "clang++"
compiler = "g++"
is_debug = 0
helper.exec_cmd(f'scons -j4 -Q debug={is_debug} -Q script={script} -Q compiler={compiler}')
helper.exec_cmd('build/HPW')
