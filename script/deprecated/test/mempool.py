#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from .. import helper

script = "test/mempool/SConscript"
is_debug = 1
helper.exec_cmd(f'scons -j4 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd('build/HPW')
