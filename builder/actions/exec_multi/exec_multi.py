import sys
from concurrent.futures import ThreadPoolExecutor, as_completed
from utils.ui import *
from utils.exec import *


def exec_multi(commands: list [list [str]], threads=4, timeout=60.0 * 5.0):
  '''
  Запускает команды из списка [команд, аргументы, ...] в многопотоке

  :exceptions:
    Если ошибка в одной из команд, стопорятся все команды.
  
  :return:
    [(stdout, stderr, elapsed)]
  '''

  result = []

  with ThreadPoolExecutor(max_workers=threads) as executor:
    # Запускаем все задачи
    future_to_cmd = {executor.submit(exec_cmd, cmd, timeout): cmd for cmd in commands}
        
    try:
      # Перехватываем результаты по мере их готовности
      for future in as_completed(future_to_cmd):
        cmd = future_to_cmd[future]
        cmd_str = " ".join(cmd)

        try:
          print(f"> запуск команды '{to_yellow(cmd_str)}'")
          # Если функция exec_cmd взорвалась, future.result() вызовет это исключение
          stdout, stderr, elapsed = future.result()
          if stdout:
            print(stdout.rstrip())
          if stderr:
            print(stderr.rstrip(), file=sys.stderr)
          print(to_gray(f"...выполнена за {elapsed} сек"))
          result.append((stdout, stderr, elapsed))
        
        except Exception as exc:
          err_msg = f"Ошибка при выполнении команды '{cmd_str}':\n{exc}\n"
          print(to_red(err_msg), file=sys.stderr)
          executor.shutdown(wait=False, cancel_futures=True)
          raise RuntimeError(err_msg) from exc
                    
    except Exception as ex:
      print(to_red("Работа пула потоков экстренно завершена"), file=sys.stderr)
      raise ex

    finally:
      # Гарантируем закрытие пула в любом случае
      executor.shutdown(wait=True)

  return result
