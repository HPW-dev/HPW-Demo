'''
cd proj_root
py builder/test-all.py
'''

from common.ui import *
import subprocess
import sys
from pathlib import Path

print("Запуск всех тестов...")

scripts_to_run = [
  "builder/test-ui.py",
  "builder/test-io.py",
  "builder/test-hash.py",
]

for script in scripts_to_run:
  script_path = Path(script)
  if not script_path.exists():
    raise Exception(f"Ошибка: Файл {script} не найден!")
        
  print(to_green(f"Запуск {script_path.name}..."))
    
  try:
    subprocess.run([sys.executable, str(script_path)], check=True)
  except subprocess.CalledProcessError as e:
    print(to_red(f"\n Ошибка в скрипте {script_path.name}!"))
    print(f"Код возврата: {e.returncode}")
    print(to_red("Останов тестов."))
    sys.exit(e.returncode)

print('='*50)
print(to_green('> все тесты пройдены'))
