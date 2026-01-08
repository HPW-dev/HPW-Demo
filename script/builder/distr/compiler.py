if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import sys
import script.builder.io.platform as platform
from script.builder.ui import *

def compiler_version(env):
  'узнать версию компилятора'
  try:
    out, _ = platform.exec_cmd(f"{env['CXX']} --version", True)
    return out
  except:
    print(txt_red(f'[!] error while getting CXX compiler version'))
  return None

def python_version():
  'проверка нужной версии Python'
  try:
    ver = sys.version_info
    if ver < (3,12,0):
      print('WARNING: требуется версия python не ниже 3.12.0')
    else:
      str_ver = f'{ver.major}.{ver.minor}.{ver.micro}'
      return str_ver
  except:
    print(txt_red(f'error while getting version of python'))
  return None
