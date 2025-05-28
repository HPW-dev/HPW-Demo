#!/usr/bin/env python
''' :file: скрипт удаляющий файлы билда '''

if __name__ != "__main__":
  from .helper import rem_all, rem_dir, rem_if_exist
else:
  from helper import rem_all, rem_dir, rem_if_exist

def clean():
  rem_all('**/delme.html')
  rem_all('**/*.exe')
  rem_all('**/*.elf')
  rem_all('**/*.elf32')
  rem_all('**/*.elf64')
  rem_all('**/vgcore.*')
  rem_all('**/cpplint.txt')
  rem_all('**/gmon.svg')
  rem_all('**/*.obj')
  rem_all('**/*.o')
  rem_all('**/*.os')
  rem_all('**/*.out')
  rem_all('**/*.gch')
  rem_all('**/log.txt')
  rem_all('**/*.gcda')
  rem_all('**/*.gcno')
  rem_all('**/*.hpw_replay')
  rem_all('**/imgui.ini')
  rem_all('**/cpplint.txt')
  rem_all('**/perf.data.old')
  rem_all('**/perf.data')
  rem_dir('__pycache__')
  rem_dir('script/__pycache__')
  rem_dir('pgo', True)
  rem_if_exist('build/bin/HPW.exe')
  rem_if_exist('build/bin/HPW')
  rem_if_exist('build/test.yml')
  rem_if_exist('.sconsign.dblite')
  rem_all('.tmp/*.txt')
  rem_all('.tmp/*.pickle')
  rem_dir('build/screenshots/', True)
  rem_all('build/replays/*')
  rem_dir('build/replays/', True)
  rem_dir('build/plugin/', True)
  rem_all('build/debug.txt')
  rem_all('build/data.zip')
  rem_dir('build/config/', True)
  rem_dir('build/info/', True)

if __name__ != "__main__":
  clean()
