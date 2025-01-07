#!/usr/bin/env python
import helper

opts = \
  ' -Q enable_asan=0' \
  ' -Q compiler=gcc' \
  ' -Q opt_level=debug'
NUM_THREADS = helper.get_max_threads() + 1
print(f'threads for building: {NUM_THREADS}')
helper.exec_cmd(f'scons -j{NUM_THREADS} -Q script=test/net-test/SConscript' + opts)
helper.exec_cmd('build/bin/net-test --server --port 49099')
