from dataclasses import dataclass, field


# Инфа о файлах для пересборки
@dataclass
class Rebuild_info:
  rebuild_needed          : bool = False
  deleted_files           : list[str] = field(default_factory=list)
  new_files               : list[str] = field(default_factory=list)
  modified_files          : list[str] = field(default_factory=list)
