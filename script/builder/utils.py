if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import hashlib
import re
import os
import shutil
import time
import glob
import subprocess
import zlib
import datetime
import hashlib
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

def file_sha3_512(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'sha3_512').hexdigest().upper()
  except:
    return None
  
def file_blake2b(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      return hashlib.file_digest(f, 'blake2b').hexdigest().upper()
  except:
    return None

def file_crc32(fname):
  try:
    with open(fname, 'rb', buffering=0) as f:
      crc = zlib.crc32(f.read())  
      return '{:08x}'.format(crc & 0xFFFFFFFF).upper()
  except:
    return None

def calculate_checksums(env):
  info = {}
  info['exe_crc32'] = file_crc32(env['executable_path'])
  info['exe_sha3_512'] = file_sha3_512(env['executable_path'])
  info['data_crc32'] = file_crc32(env['data_archive_path'])
  info['data_sha3_512'] = file_sha3_512(env['data_archive_path'])

  print('\n' + ':'*30 + ' КОНТРОЛЬНЫЕ СУММЫ ' + ':'*30)
  print(f'- CRC32 EXE: {  in_env(info, 'exe_crc32')}')
  print(f'- CRC32 DATA: { in_env(info, 'data_crc32')}')
  #print(f'- SHA512 EXE: { in_env(info, 'exe_sha512')}')
  #print(f'- SHA512 DATA: {in_env(info, 'data_sha512')}')
  return info

def max_threads():
  '''позволяет узнать сколько доступно потоков процессора'''
  try:
    return os.cpu_count()
  except:
    print('ошибка при получении числа потоков процессора; Будет использоваться 1 по умолчанию')
    return 1

def list2s(params: list[str], add_sym="") -> str:
  ":return: params объединённые в одну строку и начинающиеся на add_sym"
  ret = ""
  for x in params:
    ret += f'{add_sym}{x} '
  return ret.strip() # в начале и в конце убратьпробел

def find_mask(mask: str, recursive=False):
  "ищет файлы в папке подходящие по шаблону (например test/*.cpp)"
  ret = []
  for x in glob.glob(mask, recursive=recursive):
    ret.append(x)
  return ret

def extract_includes(file_path: str, max_lines=40):
  "Извлекает список файлов из директив #include в начале C++ файла"
  includes = []
  pattern = re.compile(r'^\s*#include\s*["](.*?)["]')
  try:
    with open(file_path, 'r', encoding='utf-8') as file:
      for line_number, line in enumerate(file):
        if line_number >= max_lines:
          break
        match = pattern.match(line)
        if match:
          included_file = match.group(1)
          includes.append(included_file)
  except IOError as e:
    print(f"Ошибка чтения файла {file_path}: {e}")
  
  # пути к файлам должны начинаться с рута:
  includes = list( map(
    lambda x: x if os.path.exists(x) else f'{os.path.dirname(file_path)}/{x}',
    includes
  ) )
  return includes

def os_env():
  'получить все системные переменные'
  return os.environ.copy()
