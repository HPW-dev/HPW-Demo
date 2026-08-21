from dataclasses import dataclass


# гланый набор настроек для сборки проекта
@dataclass
class Context:
  threads         : int = 0
  author          : str = 'Unknown'
  src_dir         : str = 'src/'
  tmp_dir         : str = '.tmp/'
  obj_dir         : str = '.tmp/objects/'
  build_dir       : str = 'build/'
  bin_dir         : str = 'build/bin/'
  info_dir        : str = 'build/build info/'
  assets_dst_path : str = 'build/data.zip'
  assets_src_dir  : str = 'data/'
  compiler_path   : str = 'g++'

  #need_rebuild          : bool = True # принудительный пересбор
  with_compilation      : bool = True # чтобы выключать сборку
  with_assets           : bool = True # паковать ресурсыассеты и копировать
  with_licenses         : bool = True # копировать лицухи
  with_build_info_file  : bool = True # копировать инфу о сборке
  with_print_build_info : bool = True # показывать инфу о сборке в консоли
  clear_all             : bool = False # если True, то чистит всё и прерывает сборку
