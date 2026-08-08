'''Тест очистки от файлов сборки и кэша'''

from actions.clean import *
from structs.context import *
from utils.ui import *
from utils.fs import *


print("=== Тест очистки ===")

ctx = Context()
clean(ctx)

print('='*50)
print(to_green('> тест пройден'))
