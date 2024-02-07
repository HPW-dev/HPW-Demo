import os
import time
import subprocess
import glob
import shutil

def set_work_dir(path):
  print (f'set working dir: \"{path}\"')
  os.chdir(path)

def rem_if_exist (fname):
  if os.path.exists (fname):
    print (f'remove {fname}')
    os.remove (fname)

def rem_dir (dname, ignore_errors=True):
  shutil.rmtree (dname, ignore_errors=ignore_errors, onerror=None)
  assert os.path.exists (dname) == False, f"dir \"{dname}\" is not deleted!"

def rem_all (fname_mask):
  list = glob.glob (fname_mask, recursive=True)
  for fname in list:
    rem_if_exist (fname)

def exec_cmd (cmd):
  cmd_tm_st = time.time()
  cmd = os.path.normpath (cmd) 
  print (cmd)
  subprocess.run (cmd.split(), check=True)
  cmd_tm_ed = time.time()
  print (f'e.t: { round(cmd_tm_ed - cmd_tm_st, 1) }s')
  print()

def write_game_version():
  with open(file='src/game/util/version.cpp', mode='w', newline='\n') as file:
    os.system('git describe --tags --abbrev=0 > _tmp_version_')
    version = open(file='_tmp_version_', mode='r').read()
    version = version.replace('\r\n', '')
    version = version.replace('\n', '')
    os.remove('_tmp_version_')

    file.write (
      '#include "version.hpp"\n'
      '\n'
      'const char* get_game_version() { return "' + version + '"; }\n'
    )
