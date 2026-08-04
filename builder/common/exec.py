import os
import time
import subprocess


def exec_cmd(cmd, without_print=False, with_normpath=True):
  '''
  Запускает команды

  Returns:
    вывод команды stdout и stderr
  '''
  cmd_tm_st = time.time()
  if with_normpath:
    cmd = os.path.normpath(cmd) 
  if without_print:
    result = subprocess.run(cmd.split(), check=True, text=True, capture_output=True)
  else:
    print(cmd)
    result = subprocess.run(cmd.split(), check=True)
  cmd_tm_ed = time.time()
  print(f'e.t: { round(cmd_tm_ed - cmd_tm_st, 1) }s')
  print()
  return result.stdout, result.stderr
