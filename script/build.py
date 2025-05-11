# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  print("is not a python module (try command: python -m script.build -h)")
  quit()

from .util import arg_parser

config = arg_parser.prepare_config()
