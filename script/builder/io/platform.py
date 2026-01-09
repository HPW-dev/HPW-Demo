if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import os
import time
import platform
import subprocess
import script.builder.utils as utils
import script.builder.distr.compiler as compiler
from script.builder.ui import *

def system_name():
  ''':return: Windows | Linux | Darwin (macOS)'''
  return platform.system()

def system_bitness():
  ''':return: x32 | x64'''
  return 'x32' if platform.architecture()[0] == '32bit' else 'x64'

def executable_name():
  system = system_name()
  bitness = system_bitness()
  if system == "Windows": return 'HPW.exe'
  if system == "Linux": return 'HPW.elf64' if bitness == 'x64' else 'HPW.elf32'
  return 'HPW'

def sys_info(env):
  info = utils.extend_vars()
  info["bitness"] = system_bitness()
  info["system"] = system_name()
  ver, commit_date, commit_time = utils.game_version()
  info["game_ver"] = ver
  info["commit_date"] = commit_date
  info["commit_time"] = commit_time
  info["python_ver"] = compiler.python_version()
  info['executable'] = executable_name()
  info['executable_path'] = f'{info['build_dir']}bin/{info['executable']}'
  return info

def max_threads():
  '''позволяет узнать сколько доступно потоков процессора'''
  try:
    return os.cpu_count()
  except:
    print(txt_red('[!] ошибка при получении числа потоков процессора; Будет использоваться 1 по умолчанию'))
    return 1

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

def quess_system(name: str = None, bits: str = None):
  '''угадать систему, на которой запустили сборку

  :return: win32 | lin32 | win64 | ...'''

  if name == None: name = system_name()
  if bits == None: bits = system_bitness()
  
  if bits == 'x32':
    if name == 'Windows': return 'win32'
    elif name == 'Linux': return 'lin32'
    else: quit(txt_red(f'unsupported system {name}'))
  elif bits == 'x64':
    if name == 'Windows': return 'win64'
    elif name == 'Linux': return 'lin64'
    else: quit(txt_red(f'unsupported system {name}'))
  else:
    quit(txt_red(f'unknown system bitness {bits}'))
