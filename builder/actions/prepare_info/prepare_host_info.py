from common.ui import *
from common.host import *
from common.exec import exec_cmd
import sys
import platform
import os

def compiler_version(env):
  '''узнать версию компилятора'''
  try:
    out, _ = exec_cmd(f"{env['CXX']} --version", True)
    return out.rstrip()
  except:
    print(to_red(f'Ошибка при получении версии компилятора из CXX переменной'))
  return None

def python_version():
  '''проверка нужной версии Python'''
  try:
    ver = sys.version_info
    if ver < (3,12,0):
      print(to_yellow('Требуется версия python не ниже 3.12.0'))
    else:
      str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
      return str_ver
  except:
    print(to_red('Ошибка при получении версии Python'))
  return None

def prepare_host_info() -> Host:
  '''узнать инфу о системе, с которой собирают'''
  ret = Host()

  os_name = platform.system()
  ret.system = Sys_name.windows if os_name == "Windows" else Sys_name.linux

  os_arch, _ = platform.architecture(executable=sys.executable)
  ret.bitness = Bitness.x64 if os_arch == '64bit' else Bitness.x32

  ret.env = dict(os.environ)

  ret.python_ver = python_version()
  ret.compiler_ver = compiler_version(ret.env)

  ret.threads = os.cpu_count()

  return ret