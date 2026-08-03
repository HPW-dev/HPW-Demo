import common.io as io
import common.ui as ui

print('=== Тест для common/io.py ===')

# делаем в .tmp папке файлики для теста
root = ".tmp/"
io.make_dir(root + 'delme/subdir')
assert(io.exists(root + 'delme'))
assert(io.exists(root + 'delme/subdir'))
with open(root + 'delme/a', 'w') as f: pass
with open(root + 'delme/b', 'w') as f: pass
with open(root + 'delme/subdir/c', 'w') as f: pass
with open(root + 'delme/subdir/d', 'w') as f: pass
with open(root + 'delme/subdir/1.txt', 'w') as f: pass
with open(root + 'delme/subdir/2.txt', 'w') as f: pass

print('> тест наличия файлов')
assert(io.exists(root + 'delme/a'))
assert(io.exists(root + 'delme/b'))
assert(not io.exists(root + 'delme/c'))
assert(not io.exists(root + 'delme/d'))
assert(io.exists(root + 'delme/subdir/c'))
assert(io.exists(root + 'delme/subdir/d'))
assert(not io.exists(root + 'delme/subdir/z'))

print('> тест поиска по маске *.txt')
finded = io.find(root + 'delme/subdir/*.txt')
assert(finded != [])
print(ui.to_yellow(finded))
print(f'* Относительный путь .. \"{ui.to_yellow(io.path_abs(finded[0]))}\"')
print(f'* Полный путь ......... \"{ui.to_yellow(io.path_full(finded[0]))}\"')

print('> удаление по маске *.txt')
io.remove(root + 'delme/subdir/*.txt')
assert(not io.exists(root + 'delme/subdir/1.txt'))
assert(not io.exists(root + 'delme/subdir/2.txt'))
print('> тест удалений')
io.remove(root + 'delme/subdir/c')
assert(not io.exists(root + 'delme/subdir/c'))
io.rem_if_exist(root + 'delme/subdir/z', 'тестовое сообщение')
io.remove_dir(root + 'delme')
assert(io.exists(root))
assert(not io.exists(root + 'delme'))
assert(not io.exists(root + 'delme/b'))
assert(not io.exists(root + 'delme/subdir/c'))
assert(not io.exists(root + 'delme/subdir/d'))

print('='*30)
print(ui.to_green('> тест пройден'))
