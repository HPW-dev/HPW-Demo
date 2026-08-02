if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

'''
import script.builder.utils as utils
import script.builder.io.clear as cleaner
import script.builder.io.hash as hash
import script.builder.io.fs as fs
import script.builder.info as build_info
import script.builder.distr.test_deps as test_deps
import os

# --------------------------------- init ---------------------------------
env = utils.init_env()
build_info.print_info(env) # показать итоговую сводку о билде

# --------------------------------- clean ---------------------------------
if 'clean' in env and env['clean'] == True:
  cleaner.clean_all()
  print('завершение без сборки')
  exit(os.EX_OK) # удалить что надо и выйти

# --------------------------------- build ---------------------------------
# попробовать тестовую прогу
if 'test_deps' in env and env['test_deps'] == True:
  test_deps.build_test_deps()

# TODO
sources = ["main.cpp"]
sources.extend( fs.find_mask("./test-dir/*.cpp") )
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

# посчитать хэши файлов:
env.update(hash.calculate_checksums(env))

# --------------------------------- finalize ---------------------------------
info_dir = env['info_dir']
info_file = f'{info_dir}build_info.json'
build_info.save_json(env, info_file)  # засейвить инфу о билде
fs.copy_license(info_dir)             # копировать инфу о лицензии
'''
