# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  print("is not a python module (try command: python -m script.build -h)")
  quit()

try:
  from .util import arg_parser
except:
  print('try this for start: python -m script.build -h')
  quit()

config = arg_parser.parse()
