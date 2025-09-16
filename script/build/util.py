if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import os
import shutil
import time
import glob
import subprocess
import datetime

def set_work_dir(path):
  "cd"
  print (f'set working dir: \"{path}\"')
  os.chdir(path)

def rem_if_exist(fname):
  "удалить файл, если он есть"
  if os.path.exists(fname):
    print (f'remove {fname}')
    os.remove(fname)

def rem_dir(dname, ignore_errors=True):
  "удалить папку"
  shutil.rmtree(dname, ignore_errors=ignore_errors, onerror=None)
  assert os.path.exists(dname) == False, f"dir \"{dname}\" is not deleted!"

def rem_all(fname_mask):
  "удалить по маске"
  list = glob.glob(fname_mask, recursive=True)
  for fname in list:
    rem_if_exist(fname)

def exec_cmd(cmd, without_print=False):
  '''выполнить команду

  :return: вывод команды stdout, stderr'''
  cmd_tm_st = time.time()
  cmd = os.path.normpath(cmd) 
  if without_print:
    result = subprocess.run(cmd.split(), check=True, universal_newlines=True,
      stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  else:
    print(cmd)
    result = subprocess.run(cmd.split(), check=True)
  cmd_tm_ed = time.time()
  print(f'e.t: { round(cmd_tm_ed - cmd_tm_st, 1) }s')
  print()
  return result.stdout, result.stderr

def copy_license():
  "копирует файл LICENSE.txt и NOTICE.txt в нужную папку"
  try:
    shutil.makedirs("build/info/", exist_ok=True)
    shutil.copyfile('LICENSE.txt', 'build/info/LICENSE.txt')
    shutil.copyfile('NOTICE.txt',  'build/info/NOTICE.txt')
  except:
    print("error copying license file")

def concat_strlist(strs: list[str]):
  return ' '.join(filter(None, strs))

def add_vars():
  '''добавляет дополнительные параметры в окружение SCons'''
  ret = {}
  ret['build_dir'] = 'build/'
  ret['ld_flags']  = []
  ret['cxx_flags'] = []
  ret['defines']   = []
  ret['generation_date'] = datetime.datetime.now().strftime("%d.%m.%Y")
  ret['generation_time'] = datetime.datetime.now().strftime("%H:%M:%S")
  return ret
