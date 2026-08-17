from actions.prepare_game_ver import game_version
from structs.context import *
from structs.target import *
from structs.host import *
from utils.ui import *
import json


def save_build_info(tgt: Target, ctx: Context, host: Host):
  '''сохраняет инфу о билде в .json'''

  ver, date, time = game_version(ctx)

  info = {
    'author': ctx.author,
    'compilation start': tgt.creation_time,

    'game ver': {
      'version': ver,
      'comit date': date,
      'comit time': time,
    },

    'compiler': {
      'python ver': host.python_ver.rstrip(),
      'c++ ver': translate_none(host.compiler_ver).rstrip(),
      'options': ' '.join(tgt.options),
      'defines': ' '.join(tgt.defines),
      'linked libs': ' '.join(tgt.linked_libs),
      'openmp': tgt.use_openmp,
    }
  }

  json_file = f"{ctx.info_dir}build.json"
  print(to_gray(f"Сохранение инфы о сборке в '{json_file}'..."))

  with open(json_file, "w", encoding="utf-8") as file:
    json.dump(info, file, indent=2, ensure_ascii=False)
