'''
cd proj_root
py builder/test-py
'''

from utils.fs import *
from utils.ui import *


print('=== Тест для common/py ===')

# делаем в .tmp папке файлики для теста
root = ".tmp/"
make_dir(root + 'delme/subdir')
assert(exists(root + 'delme'))
assert(exists(root + 'delme/subdir'))
with open(root + 'delme/a', 'w') as f: pass
with open(root + 'delme/b', 'w') as f: pass
with open(root + 'delme/subdir/c', 'w') as f: pass
with open(root + 'delme/subdir/d', 'w') as f: pass
with open(root + 'delme/subdir/1.txt', 'w') as f: pass
with open(root + 'delme/subdir/2.txt', 'w') as f: pass

print('> тест наличия файлов')
assert(exists(root + 'delme/a'))
assert(exists(root + 'delme/b'))
assert(not exists(root + 'delme/c'))
assert(not exists(root + 'delme/d'))
assert(exists(root + 'delme/subdir/c'))
assert(exists(root + 'delme/subdir/d'))
assert(not exists(root + 'delme/subdir/z'))

print('> тест поиска по маске *.txt')
finded = find(root + 'delme/subdir/*.txt')
assert(finded != [])
print(to_yellow(finded))
print(f'* Относительный путь .. \"{to_yellow(path_abs(finded[0]))}\"')
print(f'* Полный путь ......... \"{to_yellow(path_full(finded[0]))}\"')

print('> удаление по маске *.txt')
rem(root + 'delme/subdir/*.txt')
assert(not exists(root + 'delme/subdir/1.txt'))
assert(not exists(root + 'delme/subdir/2.txt'))
print('> тест удалений')
rem(root + 'delme/subdir/c')
assert(not exists(root + 'delme/subdir/c'))
rem_if_exist(root + 'delme/subdir/z')
rem_dir(root + 'delme')
assert(exists(root))
assert(not exists(root + 'delme'))
assert(not exists(root + 'delme/b'))
assert(not exists(root + 'delme/subdir/c'))
assert(not exists(root + 'delme/subdir/d'))

print('='*30)
print(to_green('> тест пройден'))
