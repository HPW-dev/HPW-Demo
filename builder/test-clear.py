'''Тест очистки от файлов сборки и кэша'''

from actions.clean import *
from structs.context import *
from utils.ui import *
from utils.fs import *


print("=== Тест очистки ===")

ctx = Context()
clean(ctx)
assert(not exists('.ruff_cache'))
assert(not exists(ctx.info_dir))

print('='*50)
print(to_green('> тест пройден'))
