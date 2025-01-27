#!/usr/bin/env python
import helper

opts = \
  ' -Q use_data_zip=0' \
  ' -Q use_netplay=1' \
  ' -Q enable_asan=1' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=test/connection-test/SConscript' + opts)
helper.exec_cmd('build/bin/connection-test')
