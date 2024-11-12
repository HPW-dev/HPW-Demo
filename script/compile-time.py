#!/usr/bin/env python
from glob import glob
from helper import exec_cmd

opts = "-std=c++23 -Wall -Wfatal-errors -finput-charset=UTF-8 -fextended-identifiers -m64 -O0 -g -fopenmp -DWINDOWS -DHOST_GLFW3 -DDEBUG -Isrc\\game -Isrc -Ithirdparty\\include -Ithirdparty\\include\\_windows_only -Ithirdparty\\include\\_windows_only\\GLEW"
result = []

list = glob("**/*.cpp", recursive=True)
for fname in list:
  cmd = f'gcc -ftime-report {opts} -c {fname} -o NUL'
  try:
    out, err = exec_cmd(cmd, True)
    # найти строчку с TOTAL
    for line in err.splitlines():
      if "TOTAL" in line:
        result.append(f'{fname} - {line}')
  except:
    print(f"error while processing file \"{fname}\"")

print("result:")
print(result)
