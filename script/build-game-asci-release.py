#!/usr/bin/env python
import helper

script = "src/game/SConscript"
plugin_script = "src/plugin/graphic-effect/cxx/SConscript"
is_debug = 0
disable_debug_info = 1
host = "asci"
helper.write_game_version()
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q disable_debug_info={disable_debug_info} -Q host={host} -Q script={script}')
helper.exec_cmd(f'scons -j16 -Q debug={is_debug} -Q disable_debug_info={disable_debug_info} -Q host={host} -Q script={plugin_script}')
if not is_debug:
  helper.rem_all("build/plugin/effect/*.a") # удалить ненужные .a файлы
helper.exec_cmd('build/HPW')
