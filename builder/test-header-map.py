from actions.compile.multi_incremental import make_header_map
from utils.ui import *
import utils.fs as fs


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

print(60*'=')
print(to_green('> тесты пройдены'))
