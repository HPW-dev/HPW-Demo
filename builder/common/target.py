'''Форматы итоговых бинарников и список файлов для сборки'''

from dataclasses import dataclass, field
from enum import StrEnum


class Extention(StrEnum):
  '''расширения'''
  exe   = ".exe"
  elf32 = ".elf32"
  elf64 = ".elf64"
  so    = ".so"
  dll   = ".dll"

@dataclass
class Target:
  '''инфа для сборки .exe/.dll/.elf файлов'''
  name: str
  ext: Extention = Extention.exe
  sources: list[str] = field(default_factory=list)