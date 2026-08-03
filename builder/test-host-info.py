from actions.prepare_info import *
from common.ui import *

print('=== Тест инфы о системе-сборщике ===')

info = prepare_host_info()
print(f'Система: {to_yellow(info.system)}')
print(f'Разрядность: {to_yellow(info.bitness)}')
print(f'Переменные среды:')
for key, val in info.env.items():
  print(f'* {key} = {to_yellow(val)}')

print('='*50)
print(to_green('> все тесты пройдены'))
