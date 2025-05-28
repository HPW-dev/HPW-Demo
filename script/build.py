#!/usr/bin/env python
# How to build: "cd" to project root & call "python script/build.py"

# защита от запуска скрипта как питоновского модуля
if __name__ != "__main__":
  quit("is not a python module (try command: python script/build.py -h)")

from colorama import init as cl_init
cl_init()

from util.helper import check_python_version
check_python_version()

from util.arg_parser import parse as arg_parse
from util.build_info import env_test
from util.build_info import prepare as build_prepare
from util.build_info import print_info as print_build_info
from util.builder import build
from util.launcher import launch
from util.clean import clean

config = arg_parse()
env_test()
config = build_prepare(config)
print_build_info(config)

if config.info: # print info only
  quit(0)

if config.clean:
  clean()
  
build(config)
launch(config)
