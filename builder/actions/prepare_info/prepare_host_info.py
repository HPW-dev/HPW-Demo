import platform
import sys
import os

from common.exec import exec_cmd
from common.host import *
from common.ui import *


def compiler_version(env):
  '''узнать версию компилятора'''
  try:
    out, _, _ = exec_cmd([env['CXX'], '--version'])
    return out.rstrip()
  except Exception as e:
    print(to_red(f'Ошибка при получении версии компилятора из CXX переменной:\n  {e}'))
  return None

def python_version():
  '''проверка нужной версии Python'''
  ver = sys.version_info
  if ver < (3,12,0):
    print(to_yellow('Требуется версия python не ниже 3.12.0'))
    return None
  else:
    str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
    return str_ver

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