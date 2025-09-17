if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import os
import shutil
import time
import glob
import subprocess
import datetime
import hashlib
from script.build.pretty_txt import *

def set_work_dir(path):
  "cd"
  print (f'set working dir: \"{path}\"')
  os.chdir(path)

def rem_if_exist(fname):
  "удалить файл, если он есть"
  if os.path.exists(fname):
    print (f'remove {fname}')
    os.remove(fname)

def check_dir(dir):
  '''проверяет что такая папка существует'''
  return os.path.exists(dir)

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

def copy_license(dir):
  "копирует файл LICENSE.txt и NOTICE.txt в нужную папку"
  try:
    os.makedirs(dir, exist_ok=True)
    shutil.copyfile('LICENSE.txt', f'{dir}LICENSE.txt')
    shutil.copyfile('NOTICE.txt',  f'{dir}NOTICE.txt')
    print(txt_green(f'license files saved in {dir}'))
  except:
    print(txt_red("error copying license file"))

def concat_strlist(strs: list[str]):
  return ' '.join(filter(None, strs))

def add_vars():
  '''добавляет дополнительные параметры в окружение SCons'''
  ret = {}
  ret['build_dir'] = './build/'
  ret['data_dir'] = './data/'
  ret['data_archive_path'] = f'{ret['build_dir']}data.zip'
  ret['bin_dir'] = f'{ret['build_dir']}bin/'
  ret['info_dir'] = f'{ret['build_dir']}info/'
  ret['ld_flags']  = []
  ret['cxx_flags'] = []
  ret['defines']   = []
  ret['generation_date'] = datetime.datetime.now().strftime("%d.%m.%Y")
  ret['generation_time'] = datetime.datetime.now().strftime("%H:%M:%S")

  if not check_dir(ret['build_dir']): quit(txt_red(f'директория {ret['build_dir']} не найдена'))
  if not check_dir(ret['bin_dir']): quit(txt_red(f'директория {ret['bin_dir']} не найдена'))
  return ret

def file_sha256(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'sha256').hexdigest().upper()
  except:
    return None

def calculate_checksums(env):
  info = {}
  info['exe_sha256'] = file_sha256(env['executable_path'])
  info['data_sha256'] = file_sha256(env['data_archive_path'])

  print('\n' + ':'*30 + ' КОНТРОЛЬНЫЕ СУММЫ ' + ':'*30)
  print(f'- SHA256 EXE: {in_env(info, 'exe_sha256')}')
  print(f'- SHA256 DATA: {in_env(info, 'data_sha256')}')
  return info
