#!/usr/bin/env python
from os import environ
import script.build.host_info as host_info

DefaultEnvironment(ENV=environ.copy())
env = Environment()
env["sys_info"] = host_info.prepare_all()
