'''
cd proj_root
py builder/test-exec.py
'''

from utils.ui import *
from utils.exec import exec_cmd


print("=== Тест выполнения команд системы ===")

# скрипт с пробелом в имени
script = '.tmp/test script.py'
with open(script, 'w') as f:
  f.write(R'''
import sys
print(f'a={sys.argv[1]} b={sys.argv[2]} c={sys.argv[3]}') ''')

script_delay = '.tmp/test-delay.py'
with open(script_delay, 'w') as f:
  f.write(R'''
import time
time.sleep(2) ''')

# проверка правильности команды
out, err, slapsed = exec_cmd(['py', script, '1', '2', '3'])
assert(out == 'a=1 b=2 c=3\n')
assert(err == '')
print(f'STDOUT ... {out.rstrip()}')
print(f'STDERR ... {err}')
print(f'ELAPSED .. {slapsed} s')

# проверка таймаута
try:
  out, err, slapsed = exec_cmd(['py', script_delay, '1', '2', '3'], 0.5)
  assert False # тут прога должна взорваться и не вызвать assert
except Exception as e:
  print(f'> успешный тест таймаута:\n  {e}')

try:
  out, err, slapsed = exec_cmd(['Трёх санкционных гусей раскатали'])
  assert False # тут прога должна взорваться и не вызвать assert
except Exception as e:
  print(f'> успешный тест неверной команды:\n  {e}')

print(79*'=')
print('> тесты пройдены')
