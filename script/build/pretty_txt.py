TXT_RED = '\033[31m'      # Красный текст
TXT_GREEN = '\033[32m'    # Зелёный текст
TXT_GRAY = '\033[37m'     # Серый текст
TXT_YELLOW = "\033[33m"   # Жёлтый текст
TXT_BLUE = "\033[34m"     # Синий текст
TXT_MAGENTA = "\033[35m"  # Пурпурный текст
TXT_CYAN = "\033[36m"     # Голубой текст
TXT_WHITE = "\033[37m"    # Белый текст (обычно серый)
TXT_RST = '\033[0m'       # Сброс цвета (возврат к стандартному)

def txt_yellow(txt): return f'{TXT_YELLOW}{txt}{TXT_RST}'
def txt_green(txt): return f'{TXT_GREEN}{txt}{TXT_RST}'
def txt_gray(txt): return f'{TXT_GRAY}{txt}{TXT_RST}'
def txt_white(txt): return f'{TXT_WHITE}{txt}{TXT_RST}'
def txt_blue(txt): return f'{TXT_BLUE}{txt}{TXT_RST}'
def txt_red(txt): return f'{TXT_RED}{txt}{TXT_RST}'

def in_env(env, val, txt_color=TXT_YELLOW, none_color=TXT_RED, none_val='Unknown'):
  '''если параметр val найдётся в env:dict, то в результате будет строка цвета txt_color'''
  if val in env and env[val] != None:
    return txt_yellow(f'{txt_color}{env[val]}{TXT_RST}')
  return txt_gray(f'{none_color}{none_val}{TXT_RST}')

def checkbox(env, val):
  if val in env and env[val] == True:
    return '✅'
  return '❌'