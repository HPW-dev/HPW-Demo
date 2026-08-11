'''Глаыный скрипт сборки: py builder'''

import sys
from utils.ui import *
from structs.host import *
from structs.context import *
from structs.target import *


if __name__ != "__main__":
  print(to_red('Запускать через "python builder"'), file=sys.stderr)
  sys.exit(1)

# TODO - чекаем систему
# TODO - парсим аргументы
# TODO - применяем параметры к контексту, таргету и прочему
# TODO - пишем что будет сделано
# TODO - подготавливаем билд, если нужно
# TODO - собираем нужное
# TODO - пакуем ассеты
# TODO - копируем лицухи
# TODO - сейвим инфу о сборке и показываем её
