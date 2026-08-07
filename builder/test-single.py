'''Тест компиляции одного .cpp файла'''

from actions.prepare_build import *
from actions.prepare_info import *
from structs.context import *
from actions.compile import *
from structs.target import *
from utils.exec import *
from utils.ui import *
from utils.fs import *


print("=== Тест компиляции одного .cpp ===")
host = prepare_host_info()
ctx = Context()
ctx.with_licenses = False
ctx.with_build_info_file = False
prepare_build(ctx)

tgt = Target()
tgt.name = f'{ctx.tmp_dir}single{tgt.ext}'
tgt.sources = find("builder/test-progs/single/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['-Wall', '-std=c++26'])
compile_single(tgt, ctx, host)


print('='*30)
print(to_green('> тест пройден'))

