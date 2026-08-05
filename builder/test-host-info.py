from actions.prepare_info import *
from utils.ui import *


print('=== Тест инфы о системе-сборщике ===')

info = prepare_host_info()
print(f'Система ......... {to_yellow(info.system)}')
print(f'Разрядность ..... {to_yellow(info.bitness)}')
print(f'Потоков ......... {to_yellow(info.threads)}')
print(f'Версия Python ... {translate_none(to_yellow(info.python_ver))}')
print(f'Версия компиля .. {translate_none(to_yellow(info.compiler_ver))}')
print(f'Переменные среды:')
for key, val in info.env.items():
  print(f'* {key} = {to_yellow(val)}')

print('='*50)
print(to_green('> все тесты пройдены'))
