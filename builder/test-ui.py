from common.ui import *

'''
cd proj_root
py builder/test-ui.py
'''

text = "Тест цветного текста:\n"
text += f"- строка с {to_red("красным")} текстом\n"
text += f"- строка с {to_green("зелёным")} текстом\n"
text += f"- строка с {to_blue("голубым/синим")} текстом\n"
text += f"- строка с {to_yellow("жёлтым")} текстом\n"
text += f"- строка с {to_white("белым")} текстом\n"
text += f"- {Color.GRAY}@ {Color.BLUE}@ {Color.RED}@ {Color.GREEN}@ " \
  f"{Color.YELLOW}@ {Color.WHITE}@ {Color.RST}\n"
text += 30*"=" + '\n'

# поиск параметров
esy_resy = {
  "esy": False,
  "resy": True,
  "dexy": None,
}
text += f'esy .... {in_env(esy_resy, 'esy')} (False)\n'
text += f'resy ... {in_env(esy_resy, 'resy')} (True)\n'
text += f'dresy .. {in_env(esy_resy, 'dexy')} (Unknown)\n'

# чекбоксы
text += f'esy .... {checkbox(esy_resy, 'esy')}\n'
text += f'resy ... {checkbox(esy_resy, 'resy')}\n'

print(text)
