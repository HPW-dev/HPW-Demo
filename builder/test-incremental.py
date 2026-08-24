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
fs.rem_dir(ctx.obj_dir)
fs.rem(f'{ctx.tmp_dir}{tgt.name}.json')

fs.make_dir(test_dir)
fs.make_dir(ctx.bin_dir)
fs.copy('builder/test-progs/single 2', test_dir)
tgt.name = f'test{tgt.ext}'
exe_path = f'{ctx.bin_dir}{tgt.name}'
tgt.sources = find(f"{test_dir}/**/*.cpp")
tgt.linked_libs.append('-static')
tgt.options.extend(['Wall', 'std=c++26', 'pipe', 'O0', 'g0'])

print(to_gray('> сборка...'))
result = compile_multi_incremental(tgt, ctx, host)

print(to_white('\n=== тест певой сборки ==='))
assert(result.rebuild_needed)
assert(result.new_files != [])
assert(result.modified_files == [])
assert(result.deleted_files == [])
assert exists(exe_path), f"файл '{exe_path}' должен существовать"
out, _, _ = exec_cmd([exe_path])
print(f'*результат программы: {to_green(out)}')
assert(out == '97997')

print(to_white('\n=== тест что ничего не поменялось #1 ==='))
result = compile_multi_incremental(tgt, ctx, host)
assert(not result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files == [])
assert(result.deleted_files == [])
assert exists(exe_path), f"файл '{exe_path}' должен существовать"
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '97997')

print(to_white('\n=== тест изменения .cpp файла ==='))
with open(f'{test_dir}subdir/func.cpp', 'w', encoding='utf-8') as f:
  f.write(
R'''#include "func.hpp"
int func() { return 777; }'''
  )
result = compile_multi_incremental(tgt, ctx, host)
assert(result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files != [])
assert(result.deleted_files == [])
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '777')

print(to_white('\n=== тест изменения .hpp файла #1 ==='))
with open(f'{test_dir}subdir/new_header.hpp', 'w', encoding='utf-8') as f:
  f.write(
R'''#pragma once
constexpr static const int sixseven = 67; // думайте'''
  )
with open(f'{test_dir}subdir/func.hpp', 'w', encoding='utf-8') as f:
  f.write(
R'''#pragma once
#include "new_header.hpp"
int func();
inline int func_2(int x=sixseven) { return x; }'''
  )
with open(f'{test_dir}main.cpp', 'w', encoding='utf-8') as f:
  f.write(R'''#include <cstdio>
#include "subdir/func.hpp"
int main() { std::printf("%i", func_2()); }'''
  )
result = compile_multi_incremental(tgt, ctx, host)
assert(result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files != [])
assert(result.deleted_files == [])
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '67')

print(to_white('\n=== тест изменения .hpp файла #2 ==='))
with open(f'{test_dir}subdir/new_header.hpp', 'w', encoding='utf-8') as f:
  f.write(
R'''#pragma once
constexpr static const int sixseven = 27000;'''
  )
result = compile_multi_incremental(tgt, ctx, host)
assert(result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files != [])
assert(result.deleted_files == [])
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '27000')

print(to_white('\n=== тест удаления файлов ==='))
fs.rem_dir(f'{test_dir}subdir/')
with open(f'{test_dir}main.cpp', 'w', encoding='utf-8') as f:
  f.write(R'''#include <cstdio>
int main() { std::printf("666"); }'''
  )
result = compile_multi_incremental(tgt, ctx, host)
assert(result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files != [])
assert(result.deleted_files != [])
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '666')

print(to_white('\n=== тест что ничего не поменялось #2 ==='))
result = compile_multi_incremental(tgt, ctx, host)
assert(not result.rebuild_needed)
assert(result.new_files == [])
assert(result.modified_files == [])
assert(result.deleted_files == [])
assert exists(exe_path), f"файл '{exe_path}' должен существовать"
out, _, _ = exec_cmd([exe_path])
print(f'результат программы: {to_green(out)}')
assert(out == '666')

print('='*79)
print(to_green('тест пройден'))
