if __name__ != "__main__":
  quit("Запускай этот файл через \"python build.py\"")

import os
import script.build.host_info as host_info
import script.build.arg_parser as arg_parser
import script.build.util as util
import script.build.build_info as build_info

env = os.environ.copy()
env.update( arg_parser.prepare() )
env.update( util.add_vars() )
env.update( host_info.prepare() )
if 'cxx' in env and env['cxx'] != None: env['CXX'] = env['cxx']
if 'cc' in env and env['cc'] != None: env['CC'] = env['cc']
build_info.print_info(env)
build_info.save_json(env, env['build_dir'] + 'info/build_info.json')
