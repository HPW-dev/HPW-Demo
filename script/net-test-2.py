#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_asan=0' \
  ' -Q compiler=gcc' \
  ' -Q use_netplay=1' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=test/net-test-2/SConscript' + opts)
helper.exec_cmd('build/bin/net-test-2 --server --port 49099')
