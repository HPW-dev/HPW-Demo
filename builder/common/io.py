# функции для работы с файлами

import os
import glob
import shutil
import pathlib
from common.ui import *

def rem_if_exist(fname: str, msg_if_not_exist: str = None):
  """удалить файл, если он есть"""
  if os.path.exists(fname):
    print (f'remove {to_yellow(path_abs(fname))}')
    os.remove(fname)
  elif msg_if_not_exist != None:
    print(msg_if_not_exist)

def make_dir(dir_name: str):
  try:
    path = pathlib.Path(dir_name)
    path.mkdir(parents=True, exist_ok=True)
    if not path.is_dir():
      raise
  except:
    raise Exception(f"не удалось создать папаку \"{dir_name}\"")

def exists(path: str):
  """проверяет что такая папка/файл существует"""
  return os.path.exists(path)

def remove_dir(dname: str, ignore_errors=True):
  """удалить папку"""
  shutil.rmtree(dname, ignore_errors=ignore_errors)
  assert os.path.exists(dname) == False, f"dir \"{dname}\" is not deleted!"

def remove(fname_mask: str):
  """удалить по маске"""
  list = glob.glob(fname_mask, recursive=True)
  for fname in list:
    rem_if_exist(fname)

def find(mask: str, recursive=False):
  """ищет файлы в папке подходящие по шаблону (например test/*.cpp)"""
  ret = []
  for x in glob.glob(mask, recursive=recursive):
    ret.append(x)
  return ret

def path_full(path: str):
  """конвертит пути файлов в полные системные пути"""
  return str(pathlib.Path(path).resolve())

def path_abs(path: str):
  """конвертит пути файлов в относительные системные пути"""
  return str(os.path.normpath(path))
