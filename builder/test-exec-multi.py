from actions.prepare_info import *
from actions.exec_multi import *
from structs.host import Sys_name
from utils.ui import *
import time


print("=== Тест выполнения команд в многопотоке ===")

host = prepare_host_info()
sleep_cmd = ['timeout', '/t', '2', '/nobreak']
if host.system == Sys_name.linux:
  sleep_cmd = ['sleep', '2']

commands = []
commands.append(['echo', 'output 1'])
commands.append(sleep_cmd)
commands.append(['echo', 'output 2'])
commands.append(sleep_cmd)
commands.append(['echo', 'output 3'])
commands.append(sleep_cmd)
commands.append(sleep_cmd)

start = time.perf_counter()
result = exec_multi(commands)
end = time.perf_counter()
assert end - start < 3.5, 'тесты должны проходиться быстро, несмотря на кучу задержек'

print('\n=== Вывод всех команд ===')
for out, err, elapsed in result:
  print(50*'-')
  print(f'STDOUT ... {out.rstrip()}')
  print(f'STDERR ... {err.rstrip()}')
  print(f'ELAPSED .. {elapsed} s')

print(50*'=')
print(to_green('> тесты пройдены'))
