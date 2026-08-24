from actions.prepare_build import prepare_build
from actions.prepare_info import prepare_host_info
from structs.context import Context
from actions.compile import compile_multi
from structs.target import Target
from utils.exec import exec_cmd
from utils.ui import *
from utils.fs import *


print("=== Тест компиляции .cpp файлов в многопотоке ===")
host = prepare_host_info()
ctx = Context()
ctx.threads = host.threads
ctx.with_licenses = False
ctx.with_build_info_file = False
rem_dir(ctx.obj_dir)
prepare_build(ctx)

tgt = Target()
tgt.name = f'multi{tgt.ext}'
ctx.bin_dir = ctx.tmp_dir
tgt.sources = find("builder/test-progs/multi/**/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['Wall', 'std=c++26', 'pipe'])
compile_multi(tgt, ctx, host)

exe_path = f'{ctx.tmp_dir}{tgt.name}'
assert exists(exe_path), f"файл '{exe_path}' должен существовать"
out, _, _ = exec_cmd([exe_path])
print(f"результат программы: '{to_green(out)}'")
assert(out == 'result: 2366537086')

print('='*79)
print(to_green('тест пройден'))
