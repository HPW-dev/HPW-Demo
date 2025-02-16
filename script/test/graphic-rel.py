#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from .. import helper

script = "test/graphic/SConscript"
#compiler = "clang++"
compiler = "g++"
is_debug = 0
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q script={script} -Q compiler={compiler}')
helper.exec_cmd('build/bin/HPW')
