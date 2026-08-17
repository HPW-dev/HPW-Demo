from actions.prepare_game_ver import *
from structs.context import *
from utils.ui import *
from utils.fs import exists


print("=== Тест генерации версии игры ===")
ctx = Context()
ver, date, time = game_version(ctx)
print(f'Версия игры ... {translate_none(ver)}')
print(f'Дата комита ... {translate_none(date)}')
print(f'Время комита .. {translate_none(time)}')

generate_game_version_file(ctx)
ver_file = f'{ctx.src_dir}/game/util/version.cpp'
assert exists(ver_file), 'Не удалось сгенерировать файлы версии игры'
with open(ver_file, 'r', encoding='utf-8') as f:
  print(f'Содержимое файла версии игры:\n{60*'='}\n{to_yellow(f.read().rstrip())}')

print(60*'=')
print(to_green('> тесты пройдены'))
