if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

import os
import script.build.host_info as host_info
import script.build.arg_parser as arg_parser
import script.build.util as util
import script.build.build_info as build_info

env = os.environ.copy()
env.update( arg_parser.prepare() )
env.update( host_info.prepare() )
if 'cxx' in env and env['cxx'] != None: env['CXX'] = env['cxx']
if 'cc' in env and env['cc'] != None: env['CC'] = env['cc']
env["compiler_ver"] = host_info.compiler_version(env)
build_info.print_info(env)

# TODO build

env.update( util.calculate_checksums(env) )
info_dir = env['info_dir']
info_file = f'{info_dir}build_info.json'
build_info.save_json(env, info_file)
util.copy_license(info_dir)
