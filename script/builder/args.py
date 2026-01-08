if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")
 
import argparse

def prepare():
  ''' Парсит команды запуска
  :return: словарь ключей и значений '''

  p = argparse.ArgumentParser(
    prog='scons',
    description='Сборщик H.P.W',
    epilog='Copyright (c) 2020-2026 HPW-dev <hpwdev0@gmail.com>',
  )

  p.add_argument('--threads', default=0, type=int, help="число потоков для сборки (0 - авто)")
  #p.add_argument('-s', '--silent', action='store_true', help='собирать всё без вывода в консоль')
  #p.add_argument('-c', '--clean', action='store_true', help='очищает папки от .o/.exe/.dll файлов')
  p.add_argument('--build_author', type=str, help='позволяет указать автора сборки')
  p.add_argument('--info', action='store_true', help='позволяет просмотреть информацию о билде без самой сборки')
  p.add_argument('--target', type=str, help='выбор целевой архитектуры: win32_i386, win32_core2, '
    'win32_atom, win32_stable, win64_stable, win64_v4, lin32_stable, lin64_stable, lin64_v4')
  p.add_argument('--disabled',  nargs='+', help='выключает опции: omp, replay, netplay, audio, pge')
  p.add_argument('--asan', action='store_true', help='включает мем-санитайзер ASAN')
  p.add_argument('--host', type=str, default='glfw3', help='выбор хост-фреймворка: '
    'sdl2, sdl3, glfw3, null, asci')
  p.add_argument('--cxx', type=str, help='позволяет выбрать свой C++ компилятор')
  p.add_argument('--cc', type=str, help='позволяет выбрать свой C компилятор')
  p.add_argument('--tests', nargs='+', help='включает тесты: graphic, math, random, sound, yaml, '
    'file-io, network')
  p.add_argument('--soft', nargs='+', help='сборка дополнительных программ')
  
  return vars(p.parse_args())
