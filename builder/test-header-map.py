from actions.compile.multi_incremental import make_header_map
from utils.ui import *


print("=== Тест поиска зависимостей в .cpp/.hpp ===")

search_dir = "builder/test-progs/includes/"
map = make_header_map(search_dir)
print(to_yellow('карта зависимостей:'))
for entry in map.items():
  print(to_yellow(entry))

print(60*'=')
print(to_green('> тесты пройдены'))
