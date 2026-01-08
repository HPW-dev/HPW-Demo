if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

# --------------------------------- init ---------------------------------
import os
import script.builder.distr.hosts as hosts
import script.builder.args as args
import script.builder.utils as utils
import script.builder.info as build_info

env = os.environ.copy()       # в системе уже могут быть свои переменные
env.update(args.prepare())    # получить параметры с аргументов запуска
env.update(hosts.prepare())   # узнать параметры у системы

# переопределить настройки компилятора:
if 'cxx' in env and env['cxx'] != None: env['CXX'] = env['cxx']
if 'cc'  in env and env['cc']  != None: env['CC']  = env['cc']
if 'ld'  in env and env['ld']  != None: env['LD']  = env['ld']
env["compiler_ver"] = hosts.compiler_version(env)

# если указано 0 потоков, то взять оптимальное число
if env['threads'] <= 0: env['threads'] = utils.max_threads() + 1

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
env.update(utils.calculate_checksums(env))

# --------------------------------- finalize ---------------------------------
info_dir = env['info_dir']
info_file = f'{info_dir}build_info.json'
build_info.save_json(env, info_file)  # засейвить инфу о билде
utils.copy_license(info_dir)          # копировать инфу о лицензии
