if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import os
import glob
import shutil
from script.builder.ui import *

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
  'проверяет что такая папка существует'
  return os.path.exists(dir)

def rem_dir(dname, ignore_errors=True):
  "удалить папку"
  shutil.rmtree(dname, ignore_errors=ignore_errors)
  assert os.path.exists(dname) == False, f"dir \"{dname}\" is not deleted!"

def rem_all(fname_mask):
  "удалить по маске"
  list = glob.glob(fname_mask, recursive=True)
  for fname in list:
    rem_if_exist(fname)

def find_mask(mask: str, recursive=False):
  "ищет файлы в папке подходящие по шаблону (например test/*.cpp)"
  ret = []
  for x in glob.glob(mask, recursive=recursive):
    ret.append(x)
  return ret

def copy_license(dir):
  "копирует файл LICENSE.txt и NOTICE.txt в нужную папку"
  try:
    os.makedirs(dir, exist_ok=True)
    shutil.copyfile('LICENSE.txt', f'{dir}LICENSE.txt')
    shutil.copyfile('NOTICE.txt',  f'{dir}NOTICE.txt')
    print(txt_green(f'license files saved in {dir}'))
  except:
    print(txt_red("error copying license file"))
