from structs.context import *
from utils.ui import *
from utils.fs import *
from actions.prepare_build import *


print("=== Тест подготовки к сборке ===")
ctx = Context()
prepare_build(ctx)

assert(exists(ctx.build_dir))
assert(exists(ctx.bin_dir))
assert(exists(f'{ctx.info_dir}NOTICE.txt'))
assert(exists(f'{ctx.info_dir}LICENSE.txt'))
print(to_green('> тест пройден'))
