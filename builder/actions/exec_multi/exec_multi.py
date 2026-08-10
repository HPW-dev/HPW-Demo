import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from utils.ui import *
from utils.exec import *


def exec_multi(commands: list [list [str]], threads=4, timeout=60.0 * 5.0):
  '''
  Запускает команды из списка [команд, аргументы, ...] в многопотоке

  :exceptions:
    Если ошибка в одной из команд, стопорятся все команды.
  '''

  with ThreadPoolExecutor(max_workers=threads) as executor:
    # Запускаем все задачи
    future_to_cmd = {executor.submit(exec_cmd, cmd, timeout): cmd for cmd in commands}
        
    try:
      # Перехватываем результаты по мере их готовности
      for future in as_completed(future_to_cmd):
        cmd = future_to_cmd[future]
        cmd_str = " ".join(cmd)

        try:
          # Если функция exec_cmd взорвалась, future.result() вызовет это исключение
          stdout, stderr, elapsed = future.result()
          if stdout:
            print(stdout.rstrip())
          if stderr:
            print(stderr.rstrip(), file=sys.stderr)
          print(to_green(f"Команда: '{cmd_str}' успешно выполнена ({elapsed} сек)"))
        except Exception as exc:
          print(to_red(f"Ошибка при выполнении команды '{cmd_str}':\n{exc}\n"), file=sys.stderr)
          executor.shutdown(wait=False, cancel_futures=True)
          raise exc
                    
    except Exception:
      print(to_red("Работа пула потоков экстренно завершена."), file=sys.stderr)
