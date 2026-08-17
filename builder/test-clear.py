'''Тест очистки от файлов сборки и кэша'''

from actions.clean import *
from structs.context import *
from utils.ui import *
from utils.fs import *


print("=== Тест очистки ===")

ctx = Context()
clean(ctx)
assert(not exists('.ruff_cache'))
assert(not exists(f'{ctx.bin_dir}HPW'))
assert(not exists(f'{ctx.bin_dir}HPW.exe'))
assert(not exists(f'{ctx.bin_dir}HPW.elf32'))
assert(not exists(f'{ctx.bin_dir}HPW.elf64'))
assert(not exists(ctx.info_dir))
assert(not exists(f'{ctx.build_dir}config'))
assert(not exists(f'{ctx.src_dir}game/util/version.cpp'))

print('='*50)
print(to_green('> тест пройден'))
