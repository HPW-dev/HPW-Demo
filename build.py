if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

import script.builder.distr.hosts as hosts
import script.builder.utils as utils
import script.builder.io.hash as hash
import script.builder.io.fs as fs
import script.builder.info as build_info

# --------------------------------- init ---------------------------------
env = utils.init_env()
build_info.print_info(env) # показать итоговую сводку о билде

# --------------------------------- build ---------------------------------
# TODO
'''
sources = ["main.cpp"]
sources.extend( utils.find_mask("./test-dir/*.cpp") )
builder = Builder(
  target = "./bin/test.exe",
  sources = sources,
  cxx_opts = ["Os", "g0", "std=c++23", "pipe"],
  defines = ["NDEBUG", "ECOMEM"],
  ld_opts = ["static", 's'],
  #libs = [],
  lib_path = ["."],
  temp = ".tmp/",
  include = ["."],
  #without_print=False
)
builder.run()
'''

# посчитать хэши файлов:
env.update(hash.calculate_checksums(env))

# --------------------------------- finalize ---------------------------------
info_dir = env['info_dir']
info_file = f'{info_dir}build_info.json'
build_info.save_json(env, info_file)  # засейвить инфу о билде
fs.copy_license(info_dir)             # копировать инфу о лицензии
