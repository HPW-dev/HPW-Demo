from dataclasses import dataclass, field
from enum import StrEnum

class Extention(StrEnum):
  exe   = ".exe"
  elf32 = ".elf32"
  elf64 = ".elf64"
  so    = ".so"
  dll   = ".dll"

# инфа для сборки .exe/.dll/.elf файлов
@dataclass
class Target:
  name: str
  ext: Extention = Extention.exe
  sources: list[str] = field(default_factory=list)