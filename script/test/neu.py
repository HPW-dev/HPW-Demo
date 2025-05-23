#!/usr/bin/env python
import helper

opts = \
  ' -Q use_data_zip=0' \
  ' -Q use_netplay=0' \
  ' -Q enable_asan=0' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=test/neu-test/SConscript' + opts)
helper.exec_cmd('build/bin/neu-test')
