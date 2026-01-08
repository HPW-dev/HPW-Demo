if __name__ == "__main__":
  quit("Запускать через \"python build.py\"")

import script.builder.utils as util
from script.builder.ui import *
import json
import os
import datetime

# сборщик проекта
class Builder:
  def __init__(self,
    target: str,
    sources: list[str],
    cxx_opts: list[str]=[],
    defines: list[str]=[],
    ld_opts: list[str]=[],
    libs: list[str]=[],
    lib_path: list[str]=[],
    include: list[str]=[],
    temp: str="", 
    cxx: str="g++",
    without_print: bool=True
  ):
    assert sources
    assert target
    self.__sources = sources
    self.__target = target
    self.__db_path = ".tmp/building-map.json"
    self.__db = self.__open_db(self.__db_path)
    self.__defines = util.list2s(defines, '-D')
    self.__cxx_opts = util.list2s(cxx_opts, '-')
    self.__ld_opts = util.list2s(ld_opts, '-')
    self.__libs = util.list2s(libs, '-l')
    self.__include = util.list2s(include, '-I')
    self.__lib_path = util.list2s(lib_path, '-L')
    self.__without_print = without_print
    self.__temp = temp
    self.__cxx = cxx
    self.__exe_rebuild = False
    self.__update_compilation_info()
    self.__db_save()

    if not self.__without_print:
      print(f"- Sources: {self.__sources}")
      print(f"- Defines: {self.__defines}")
      print(f"- C++ options: {self.__cxx_opts}")
      print(f"- Linker options: {self.__ld_opts}")
      print(f"- Used LIB's: {self.__libs}")
      print(f"- Include path: {self.__include}")
      print(f"- LIB's path: {self.__lib_path}")
  
  def __open_db(self, path: str):
    "открывает JSON, если его нету, то создаёт новый"
    try:
      if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
          return json.load(f)
      else:
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w', encoding='utf-8') as f:
          json.dump({}, f, ensure_ascii=False, indent=2)
        db = {}
        db['source_files'] = {} # кэши файлов .cpp
        return db
    except Exception as e:
      quit(f'Error while opening file \"{path}\": {e}')
    return None

  def __file_info(self, fname: str):
    assert(fname)
    mtime = os.path.getmtime(fname)
    modifed_time = datetime.datetime.fromtimestamp(mtime).isoformat()
    return {
      #'path': fname,
      #'obj': self.__to_obj_name(fname),
      'date': modifed_time,
      'size': os.path.getsize(fname),
      'hash': util.file_blake2b(fname),
      'includes': util.extract_includes(fname),
    }
  
  def __file_to_db(self, fname: str):
    "добавляет инфу о файле в кэш"
    assert(self.__db != None)
    file_info = self.__file_info(fname)
    for inc in file_info['includes']:
      self.__file_to_db(inc)
    self.__db['source_files'][fname] = file_info

  def __to_obj_name(self, name: str):
    "конвертирует .c/.cpp файлы в .o и сепараторы заменяет на _"
    ret = name
    ret = ret.replace('./', '')
    ret = ret.replace('.', '_')
    ret = ret.replace('/', '_')
    ret = ret.replace('.cpp', '')
    ret = ret.replace('.c', '')
    ret = self.__temp + ret
    return f'{ret}.o'

  def __check_includes(self, info) -> bool:
    # проверить что среди включаемых файлов есть изменения:
    for inc in info['includes']:
      inc_info = self.__file_info(inc)
      if inc_info != self.__db['source_files'].get(inc):
        if not self.__without_print:
          print(f'обнаружены изменения в файле \"{inc}\"')
        return True
      if (self.__check_includes(inc_info)):
        return True
    return False

  def __need_obj_compile(self, path: str) -> bool:
    ":return: нужно ли компилить .o файл"
    # если изменений не было, то игнор
    file_info = self.__db['source_files'].get(path)
    if self.__file_info(path) == file_info:
      if os.path.exists(self.__to_obj_name(path)): # проверить что есть такой .o файл
        if not self.__check_includes(file_info):
          return False
    return True
  
  def __need_exe_compile(self, path: str) -> bool:
    ":return: нужно ли линковать экзешник"
    return self.__exe_rebuild

  def run(self):
    "запуск сборки"

    objects = []
    # сборка .o модулей
    for src in self.__sources:
      src = src.replace('\\', '/')
      src = src.replace('./', '')
      obj_name = self.__to_obj_name(src)
      objects.append(obj_name)

      if self.__need_obj_compile(src):
        if not self.__without_print:
          print(f'Сборка файла \"{src}\"')
        self.__exe_rebuild = True
        try:
          _, err = util.exec_cmd(f'{self.__cxx} -c -o {obj_name} {self.__defines} {self.__cxx_opts} '
            f'{self.__ld_opts} {self.__include} {self.__lib_path} {src}')
          if err:
            raise err
          self.__file_to_db(src)
        except Exception as e:
          quit(f"error while object compile: {e}")
      elif not self.__without_print:
          print(f'В файле \"{src}\" нет изменений. Игнор')
    
    if not self.__without_print:
      print(f"objects: {objects}")

    # финальная линковка:
    if self.__need_exe_compile(self.__target):
      print(f'Линковка файла \"{self.__target}\"')
      try:
        _, err = util.exec_cmd(f'{self.__cxx} -o {self.__target} {self.__defines} {self.__cxx_opts} '
          f'{self.__ld_opts} {self.__temp}*.o {self.__lib_path} {self.__libs}')
        if err:
          raise err
      except Exception as e:
        quit(f"error while linking: {e}")
    else:
      print(f'Линковка файла \"{self.__target}\" не требуется')
    
    # засевить всё после билда
    self.__db_save()

  def __update_compilation_info(self):
    "добавляет инфу о опциях компиляции в кэш сборки"
    assert(self.__db != None)
    self.__db["compile_info"] = {
      "cxx": self.__cxx,
      "defines": self.__defines,
      "ld_opts": self.__ld_opts,
      "cxx_opts": self.__cxx_opts,
      "include": self.__include,
      "lib_path": self.__lib_path,
      "libs": self.__libs,
    }
  
  def __db_save(self):
    assert(self.__db != None)
    assert(self.__db_path)
    with open(self.__db_path, 'w', encoding='utf-8') as f:
      json.dump(self.__db, f, ensure_ascii=False, indent=2)
