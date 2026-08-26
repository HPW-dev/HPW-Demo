from actions.compile.multi_incremental import make_header_map
from utils.ui import *
from utils import fs


print("=== Тест поиска зависимостей в .cpp/.hpp ===")

search_dir = "builder/test-progs/includes/"
cxx_files = []
cxx_files.extend(fs.find(f'{search_dir}**/*.cxx'))
cxx_files.extend(fs.find(f'{search_dir}**/*.cpp'))
cxx_files.extend(fs.find(f'{search_dir}**/*.cc'))
cxx_files.extend(fs.find(f'{search_dir}**/*.c'))
map = make_header_map(cxx_files)

print(to_yellow('карта зависимостей:'))
for entry in map.items():
  print(to_yellow(entry))

# проверки что нужные файлы найдены:
assert(fs.path_abs('builder/test-progs/includes/subdir/a.cxx') in map)
assert(fs.path_abs('builder/test-progs/includes/root.c') in map)
assert(fs.path_abs('builder/test-progs/includes/root.h') in \
  map[fs.path_abs('builder/test-progs/includes/root.c')]['headers'])
assert(fs.path_abs('builder/test-progs/includes/subdir/b.hpp') in \
  map[fs.path_abs('builder/test-progs/includes/subdir/a.cxx')]['headers'])

print(60*'=')
print(to_green('> тесты пройдены'))
