#!/usr/bin/env python
import helper

script = "src/game/SConscript"
is_debug = 0
helper.write_game_version()
helper.exec_cmd(f'scons -j4 -Q debug={is_debug} -Q script={script}')
helper.exec_cmd('build/HPW')
