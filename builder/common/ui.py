# Для красивых логов

from enum import StrEnum

class Color(StrEnum):
  RED     = '\033[31m' # Красный текст
  GREEN   = '\033[32m' # Зелёный текст
  GRAY    = '\033[90m' # Серый текст
  YELLOW  = "\033[33m" # Жёлтый текст
  BLUE    = "\033[34m" # Синий текст
  MAGENTA = "\033[35m" # Пурпурный текст
  CYAN    = "\033[36m" # Голубой текст
  WHITE   = "\033[37m" # Белый текст (обычно серый)
  RST     = '\033[0m'  # Сброс цвета (возврат к стандартному)

def to_yellow(txt): return f'{Color.YELLOW}{txt}{Color.RST}'
def to_green(txt):  return f'{Color.GREEN}{txt}{Color.RST}'
def to_gray(txt):   return f'{Color.GRAY}{txt}{Color.RST}'
def to_white(txt):  return f'{Color.WHITE}{txt}{Color.RST}'
def to_blue(txt):   return f'{Color.BLUE}{txt}{Color.RST}'
def to_red(txt):    return f'{Color.RED}{txt}{Color.RST}'

def in_env(env, val, finded_color=Color.YELLOW, none_color=Color.RED, none_val='Unknown'):
  '''если параметр val найдётся в env:dict, то в результате будет строка цвета finded_color'''
  if val in env and env[val] != None:
    return to_yellow(f'{finded_color}{env[val]}{Color.RST}')
  return to_gray(f'{none_color}{none_val}{Color.RST}')

def checkbox(env, val):
  '''если параметр val найдётся в env:dict и будет True, то показать галочку'''
  if val in env and env[val] == True:
    return '✅'
  return '❌'
