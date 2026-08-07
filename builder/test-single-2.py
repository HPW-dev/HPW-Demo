'''Тест компиляции .cpp файлов'''

from actions.prepare_build import *
from actions.prepare_info import *
from structs.context import *
from actions.compile import *
from structs.target import *
from utils.exec import *
from utils.ui import *
from utils.fs import *


print("=== Тест компиляции .cpp файлов ===")
host = prepare_host_info()
ctx = Context()
ctx.with_licenses = False
ctx.with_build_info_file = False
prepare_build(ctx)

tgt = Target()
tgt.name = f'{ctx.tmp_dir}single2{tgt.ext}'
tgt.sources = find("builder/test-progs/single 2/**/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['-Wall', '-std=c++26'])
compile_single(tgt, ctx, host)

assert(exists(tgt.name))
out, _, _ = exec_cmd([tgt.name])
assert(out == '97997')
print(f'результат программы: {to_green(out)}')

print('='*50)
print(to_green('> тест пройден'))
