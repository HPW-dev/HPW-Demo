from dataclasses import dataclass, field
from enum import StrEnum

class Sys_name(StrEnum):
  linux = 'Linux'
  windows = 'Windows'

class Bitness(StrEnum):
  '''разрядность'''
  x32 = 'x32'
  x64 = 'x64'

@dataclass
class Host:
  '''инфа о системе, с которой собирают'''
  bitness: Bitness = Bitness.x32
  system: Sys_name = Sys_name.windows
  env: dict = field(default_factory=dict) # переменные среды
