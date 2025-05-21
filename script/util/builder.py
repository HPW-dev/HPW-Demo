from util import helper
import random

build_dir = "build/"
bin_dir = build_dir + "bin/"
test_dir = 'test/'
src_dir = 'src/'

def compilation_test():
  print('\n--------------------------------{ Compilation test }--------------------------------')
  cxx_target = bin_dir + "cxx-test.exe"
  sources = test_dir + "cxx/main.cpp"
  cmd = 'scons'
  cmd += f' PROG_NAME={cxx_target}'
  cmd += f' SOURCES={sources}'
  cmd += f' HOST_SYSTEM={config.system}'
  helper.exec_cmd(cmd)

  test_val = random.randint(1, 999999)
  helper.exec_cmd(f'{cxx_target} {test_val}')
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
  compilation_test()

  print('\n--------------------------------{ Build H.P.W }--------------------------------')
  # TODO передать конфиг в sconscript и скопировать лицензии с ресурсами куда надо
  