#!/usr/bin/env python
# How to build: "cd" to project root & call "python script/build.py"

# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  quit("is not a python module (try command: python script/build.py -h)")

import colorama
colorama.init()

from util import helper
helper.check_python_version()

from util import arg_parser
from util import build_info
from util import builder
from util import launcher

config = arg_parser.parse()
build_info.env_test()
config = build_info.prepare(config)
build_info.print_info(config)

if config.info: # print info only
  quit(0)

builder.build(config)
launcher.launch(config)
