#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from ... import helper

helper.exec_cmd(f'scons -j4 -Q debug=0 -Q script=tool/table-gen/SConscript')
helper.exec_cmd("build/table gen")
