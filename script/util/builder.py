from util.helper import exec_cmd
from random import randint
from colorama import Fore, Style

build_dir = "build/"
bin_dir = build_dir + "bin/"
test_dir = 'test/'
src_dir = 'src/'

def compilation_test(config):
  print('\n--------------------------------{ Compilation test }--------------------------------')
  cxx_target = bin_dir + "cxx-test.exe"
  sources = [test_dir + "cxx/main.cpp"]
  cmd = 'scons'
  cmd += f' PROG_NAME={cxx_target}'
  cmd += f' SOURCES={'@'.join(sources)}'
  cmd += f' HOST_SYSTEM={config.system}'
  cmd += f' LD_FLAGS={'@'.join(config.ld_flags)}'
  cmd += f' CXX_FLAGS={'@'.join(config.cxx_flags)}'
  try:
    exec_cmd(cmd)
  except:
    quit(Fore.RED + 'error while compilation test app' + Style.RESET_ALL)

  test_val = randint(1, 999999)
  exec_cmd(f'{cxx_target} {test_val}')
  readed_val = -1
  with open('.tmp/empty-prog-question.txt', 'r') as file:
    line = file.readline()
    readed_val = int(line)
  print(f'validation value: {test_val} / infile value: {readed_val}')
  if readed_val == test_val:
    print('C++ test status: success')
  else:
    quit('cxx test status: failure')

def build(config):
  compilation_test(config)

  print('\n--------------------------------{ Build H.P.W }--------------------------------')
  # TODO передать конфиг в sconscript и скопировать лицензии с ресурсами куда надо
  