#!/usr/bin/env python
if __name__ != "__main__":
  print("is not a python module")

from .. import helper

opts = \
  ' -Q use_data_zip=0' \
  ' -Q use_netplay=1' \
  ' -Q enable_asan=0' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=test/net-test/SConscript' + opts)
helper.exec_cmd('build/bin/net-test')
