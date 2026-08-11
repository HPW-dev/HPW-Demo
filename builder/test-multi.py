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
prepare_build(ctx)

tgt = Target()
tgt.name = f'{ctx.tmp_dir}multi{tgt.ext}'
tgt.sources = find("builder/test-progs/multi/**/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['-Wall', '-std=c++26', '-pipe'])
compile_multi(tgt, ctx, host)

assert exists(tgt.name), f"файл '{tgt.name}' должен существовать"
out, _, _ = exec_cmd([tgt.name])
assert(out == 'result: 2366537086')
print(f"результат программы: '{to_green(out)}'")

print('='*79)
print(to_green('> тест пройден'))
