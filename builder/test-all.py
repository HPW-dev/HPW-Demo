'''
cd proj_root
py builder/test-all.py
'''

import subprocess
import sys
from pathlib import Path


print("Запуск всех тестов...")

scripts_to_run = [
  "builder/test-ui.py",
  "builder/test-fs.py",
  "builder/test-hash.py",
  "builder/test-exec.py",
  "builder/test-host-info.py",
  "builder/test-build-info.py",
]

for script in scripts_to_run:
  script_path = Path(script)
  if not script_path.exists():
    raise Exception(f"Ошибка: Файл {script} не найден!")
        
  print(f"Запуск {script_path.name}...")
    
  try:
    subprocess.run([sys.executable, str(script_path)], check=True)
  except subprocess.CalledProcessError as e:
    print(f"\n Ошибка в скрипте {script_path.name}!")
    print(f"Код возврата: {e.returncode}")
    print("Останов тестов.")
    sys.exit(e.returncode)

print('='*50)
print('> все тесты пройдены')
