from common.host import *
import platform
import sys
import os

def prepare_host_info() -> Host:
  '''узнать инфу о системе, с которой собирают'''
  ret = Host()

  os_name = platform.system()
  ret.system = Sys_name.windows if os_name == "Windows" else Sys_name.linux

  os_arch, _ = platform.architecture(executable=sys.executable)
  ret.bitness = Bitness.x64 if os_arch == '64bit' else Bitness.x32

  ret.env = dict(os.environ)

  return ret