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
  defines       : list[str] = field(default_factory=list)
  options       : list[str] = field(default_factory=list)
  include_dirs  : list[str] = field(default_factory=list)
  sources       : list[str] = field(default_factory=list)
  lib_dirs      : list[str] = field(default_factory=list)
  ext           : Extention = Extention.exe
  name          : str = "test.exe"
  pch_path      : str = '' # берётся из Context. Есзи задан, будет билд PCH
  opt_preset    : str = 'auto' # уровень оптимизаций
  game_ver_file : str = '' # Если задан путь к .rc файлу, то будет вшиваться версия
  linked_libs   : list[str] = field(default_factory=list)
  creation_time : str = "" # время старта сборки (UTC)
  use_openmp    : bool = True