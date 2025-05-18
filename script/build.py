# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  print("is not a python module (try command: python -m script.build -h)")
  quit()

try:
  from .util import helper
  helper.check_python_version()

  from .util import arg_parser
  from .util import build_info
  from .util import builder
  from .util import launcher

  import colorama
  colorama.init()
except:
  print(colorama.Fore.RED + 'try this for start: python -m script.build -h' + colorama.Style.RESET_ALL)
  quit()

config = arg_parser.parse()
config = build_info.prepare(config)
build_info.print_info(config)

if config.info: # print info only
  quit()

builder.build(config)
launcher.launch(config)
