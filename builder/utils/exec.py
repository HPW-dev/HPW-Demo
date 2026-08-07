import time
import subprocess


def exec_cmd(cmd: list [str], timeout: float = 60.0 * 2) -> tuple[str, str, float]:
  '''
  Запускает команды

  Returns:
    (stdout, stderr, elapsed_time_sec)
  '''

  cmd_tm_st = time.perf_counter()
  stdout, stderr = "", ""

  try:
    result = subprocess.run(cmd,
      check=True,          # исключение, если retcode не 0
      text=True,           # байты в строку
      capture_output=True, # забрать STDOUT/STDERR
      timeout=timeout)
    stdout, stderr = result.stdout, result.stderr

  except subprocess.CalledProcessError as e:
    # 4. Обработка ошибок упавшей команды
    stdout, stderr = e.stdout, e.stderr
    raise Exception(
      f'не удалось выполнить команду {cmd}\n' \
      f'* Код возврата {e.returncode}\n' \
      f'* STDERR: {e.stderr}\n' \
      f'* STDOUT: {e.stdout}') from e

  # обработка таймаута
  except subprocess.TimeoutExpired as e:
    stdout = (e.stdout or b"").decode() if isinstance(e.stdout, bytes) else (e.stdout or "")
    stderr = (e.stderr or b"").decode() if isinstance(e.stderr, bytes) else (e.stderr or "")
    raise TimeoutError(f"Превышено время ожидания для команды \"{' '.join(cmd)}\" ({timeout} сек)") from e
  
  except FileNotFoundError as e:
    raise FileNotFoundError(f'программа \"{cmd[0] if cmd else cmd}\" не найдена') from e
  
  finally:
    cmd_tm_ed = time.perf_counter()
    slapsed = round(cmd_tm_ed - cmd_tm_st, 2)

  return stdout, stderr, slapsed
