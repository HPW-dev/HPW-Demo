#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from ... import helper

script = "tool/code-to-image/SConscript"
is_debug = 0
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd('build/code-to-image .')
