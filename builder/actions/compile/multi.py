from actions.exec_multi import *
from structs.target import *
from structs.context import *
from structs.host import *
from utils.exec import exec_cmd
from utils.ui import *
from utils import fs


def compile_multi(tgt: Target, ctx: Context, host: Host):
  '''Многопоточная компиляция без инкрементальной сборки'''
  print(f'Сборка \'{to_yellow(tgt.name)}\'...')
  pass # TODO