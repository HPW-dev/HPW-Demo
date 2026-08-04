'''Функции для работы с файлами'''

import shutil
from pathlib import Path


def rem_if_exist(path: str):
  '''Удалить файл, если он есть'''
  p = Path(path)
  if p.is_file():
    p.unlink()

def rem(path: str):
  '''Удалить файл. Если его нет, не вылетать'''
  p = Path(path)
  folder = p.parent
  mask = p.name
  if folder.exists() and folder.is_dir():
    for file in folder.glob(mask):
      if file.is_file():
        file.unlink(missing_ok=True)

def make_dir(dir_name: str):
  '''Создать папку'''
  Path(dir_name).mkdir(parents=True, exist_ok=True)

def rem_dir(dname: str, ignore_errors=True):
  '''Удалить папку'''
  shutil.rmtree(dname, ignore_errors=ignore_errors)

def find(mask: str) -> list[str]:
  '''Ищет файлы по маске (src/**/*.cpp)'''
  base = Path(".") # искать от рута
  return [str(p) for p in base.glob(mask)]

def path_full(path: str):
  '''конвертит пути файлов в полные системные пути'''
  return str(Path(path).resolve())

def path_abs(path: str):
  '''конвертит пути файлов в относительные системные пути'''
  return str(Path(path))

def exists(path: str):
  '''Проверяет что файл существует'''
  return Path(path).exists()
