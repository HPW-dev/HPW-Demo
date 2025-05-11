#!/usr/bin/env python
'''
Module usage:
  import diff_checker 
  diff_checker.compress_diffs("data/", "./data", ".tmp/data_diff.pickle")
'''

import pickle
from logging import error as log_error
from shutil import make_archive
import os.path
from zlib import crc32

type Diff_data = dict[str, tuple[float, int]];

def check_file_diffs(folder: str, cache_path: str):
  '''проверяет что в папке folder изменился хотя бы один файл'''

  # грузим базу
  diff_data: Diff_data = {}
  with open(cache_path, 'rb') as file:
    diff_data = pickle.load(file)

  # пройтись по файлам в папке и найти изменения:
  for dirname, subdirs, files in os.walk(folder):
    for filename in files:
      file_path = dirname + '/' + filename;

      if file_path in diff_data:
        time_info, file_checksum = diff_data[file_path]
        # сравниваем время создания файла
        cur_file_time_info = os.path.getmtime(file_path)
        if cur_file_time_info != time_info:
          # если чексуммы разные, то есть изменение
          cur_file_checksum_info = get_file_checksum(file_path)
          if cur_file_checksum_info != file_checksum:
            print(f'обнаружены изменения в файле \"{file_path}\"')
            return True
      else: # файла нет в базе, значит есть изменение
        print(f'добавлен новый файл \"{file_path}\"')
        return True
  
  # проверить удалённые файлы
  for file_path in diff_data:
    if not os.path.exists(file_path):
      print(f'обнаружено удаление файла \"{file_path}\"')
      return True

  return False

def get_file_checksum(path):
  file_hash = 0
  with open(path, "rb") as file:
    while chunk := file.read(1024*1024*4):
      file_hash = crc32(chunk, file_hash)
  return file_hash

def make_diff_data(folder: str, cache_path: str):
  '''сейвит базу изменений в файлах в cache_path'''
  diff_data: Diff_data = {}

  # пройтись по всем файлам и записать их данные
  for dirname, subdirs, files in os.walk(folder):
    for filename in files:
      file_path = dirname + '/' + filename;
      time_info = os.path.getmtime(file_path)
      hecksum_info = get_file_checksum(file_path)
      diff_data[file_path] = (time_info, hecksum_info)

  # засейвить базу
  with open(cache_path, 'wb') as file:
    pickle.dump(diff_data, file, protocol=pickle.HIGHEST_PROTOCOL)

def compress_folder(folder: str, archive_path: str):
  '''жмёт всю папку folder в ахив archive_path'''
  make_archive(archive_path, 'zip', folder)

def compress_diffs(folder: str, archive_path: str, cache_path: str):
  '''Отслеживает изменения в папке и сжимает её, если изменения были

  :param folder: папка, которую отслеживают и сжимают
  :param archive_path: путь к выходному архиву (без расширения)
  :param cache_path: файл, где хранится база изменений в файлах
  :return: True, если нет ошибок'''
  try:
    # проверки:
    if not folder:
      raise Exception("не указана папка для отслеживания изменений")
    if os.path.isdir(folder):
      if not os.listdir(folder):
        raise Exception(f"папка \"{folder}\" пуста")
    else:
      raise Exception(f"папка \"{folder}\" не обнаружена")
    if not archive_path:
      raise Exception("не указана папка для выходного архива")
    if not cache_path:
      raise Exception("укажите путь к базе изменений в файлах")
    
    need_archive = False
    
    # если запускаем первый раз
    if not os.path.isfile(cache_path):
      print(f'Нету базы изменений в файлах в \"{cache_path}\".')
      need_archive = True;
    # что-то поменялось в файлах
    elif check_file_diffs(folder, cache_path):
      print(f'в папке \"{folder}\" есть изменения.')
      need_archive = True;

    if need_archive:
      print(f'создание базы изменений в файлах в \"{cache_path}\"')
      make_diff_data(folder, cache_path)
      print(f'создание архива папки \"{folder}\" в \"{archive_path}.zip\"')
      compress_folder(folder, archive_path)
    else:
      print(f'изменений в \"{folder}\" нет, действия не требуются.')

    return True
  except Exception as ex:
    log_error(ex, exc_info=True)
    return False
  else:
    log_error('неизвестная ошибка', exc_info=True)
    return False
