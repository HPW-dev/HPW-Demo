from actions.compile.multi_incremental import *
from actions.prepare_info import prepare_host_info
from structs.context import Context
from structs.target import Target
from utils.exec import exec_cmd
from actions.compile import compile_multi
from actions.prepare_build import prepare_build
from utils.ui import *
from utils.fs import *


print("=== Тест инкрементальной сборки ===")

print(to_gray('> Подготовка...'))
host = prepare_host_info()
ctx = Context()
tgt = Target()
ctx.threads = host.threads
ctx.with_licenses = False
ctx.with_build_info_file = False
prepare_build(ctx)

# снести то, что уже собрано в прошлый раз
test_dir = f'{ctx.tmp_dir}multi_incremental/'
ctx.bin_dir = f'{test_dir}bin/'
fs.rem_dir(test_dir)

fs.make_dir(test_dir)
fs.make_dir(ctx.bin_dir)
fs.copy('builder/test-progs/single 2', test_dir)
tgt.name = f'test{tgt.ext}'
exe_path = f'{ctx.bin_dir}{tgt.name}'
tgt.sources = find(f"{test_dir}/**/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['Wall', 'std=c++26', 'pipe', 'O0', 'g0'])

print(to_gray('> сборка...'))
compile_multi_incremental(tgt, ctx, host)

print(to_gray('> проверка...'))
assert exists(exe_path), f"файл '{exe_path}' должен существовать"
out, _, _ = exec_cmd([exe_path])
#print(f"результат программы: '{to_green(out)}'")
#assert(out == 'result: 2366537086')

print('='*79)
print(to_green('тест пройден'))
