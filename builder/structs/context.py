from dataclasses import dataclass
from enum import StrEnum


# гланый набор настроек для сборки проекта
@dataclass
class Context:
  threads:    int = 0
  author:     str = "Unknown"
  tmp_dir:    str = ".tmp/"
  build_dir:  str = "build/"
  bin_dir:    str = "build/bin/"
  info_dir:   str = "build/build info/"

  #need_rebuild:           bool = True # принудительный пересбор
  with_compilation:       bool = True # чтобы выключать сборку
  with_assets:            bool = True # паковать ресурсыассеты и копировать
  with_licenses:          bool = True # копировать лицухи
  with_build_info_file:   bool = True # копировать инфу о сборке
  with_print_build_info:  bool = True # показывать инфу о сборке в консоли
