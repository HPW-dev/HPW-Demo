# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  print("is not a python module (try command: python -m script.build -h)")
  quit()

try:
  from .util import arg_parser
  from .util import build_info
  from .util import builder
  from .util import launcher
except:
  print('try this for start: python -m script.build -h')
  quit()

config = arg_parser.parse()
build_info.print_info(config)

if config.info: # print info only
  quit()

builder.build(config)
launcher.launch(config)
