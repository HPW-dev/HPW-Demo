if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

import os
import script.build.host_info as host_info
import script.build.arg_parser as arg_parser
import script.build.util as util
import script.build.build_info as build_info

def main():
  env = init()
  build(env)
  finalize(env)

# ------------------------------------------------------------------------------------

def init():
  env = os.environ.copy()            # в системе уже могут быть свои переменные
  env.update( arg_parser.prepare() ) # получить параметры с аргументов запуска
  env.update( host_info.prepare() )  # узнать параметры у системы

  # переопределить настройки компилятора:
  if 'cxx' in env and env['cxx'] != None: env['CXX'] = env['cxx']
  if 'cc' in env and env['cc'] != None: env['CC'] = env['cc']
  env["compiler_ver"] = host_info.compiler_version(env)

  build_info.print_info(env) # показать итоговую сводку о билде
  return env

def build(env):
  pass # TODO build
  env.update( util.calculate_checksums(env) ) # посчитать хэши файлов
  return env

def finalize(env):
  info_dir = env['info_dir']
  info_file = f'{info_dir}build_info.json'
  build_info.save_json(env, info_file) # засейвить инфу о билде
  util.copy_license(info_dir)          # копировать инфу о лицензии

# ------------------------------------------------------------------------------------

main()
