if __name__ == "__main__":
  quit('Запускать через "python build.py --test_deps"')

import script.builder.io.fs as fs
import script.builder.ui as ui

class Compiler:
  '''сборщик C++ программ'''

  def __init__(self,
    target: str,
    sources: list[str],
    cxx_opts: list[str] = [],
    defines: list[str] = [],
    ld_opts: list[str] = [],
    libs: list[str] = [],
    lib_path: list[str] = [],
    include: list[str] = [],
    temp_dir: str = "",
    log = False
  ):
    '''
    :param target: название итоговой программы/библиотеки
    :param sources: список файлов, которые надо скомпилить (для списка из файла, юзать @sources.txt)
    :param cxx_opts: опции оптимизации C++ и прочее (не для линкера)
    :param defines: дефайны (типа [-DDEBUG])
    :param ld_opts: опции только для линкера
    :param libs: какие юзать либы (типа [glfw, GL, OpenAL])
    :param lib_path: папка поиска .a/.s/.lib файлов
    :param include: папка поиска для #include C++
    :param temp_dir: в какой папке создавать временные файлы сборки
    :param log: True включает доп. принты
    '''

    self._target = fs.abs_path(target)
    self._sources = [fs.abs_path(p) for p in sources]
    self._cxx_opts = cxx_opts
    self._defines = defines
    self._ld_opts = ld_opts
    self._libs = libs
    self._lib_path = lib_path
    self._include = include
    self._temp_dir = temp_dir
    self._log = log

    assert self._target # имя проги пустым быть не может
    assert self._sources # требуется список файлов для компиляции

    if log:
      print('Compile params:')
      print(f'- target program: {self._target}')
      print(f'- source files: {self._sources}')
      print(f'- C++ options: {self._cxx_opts}')
      print(f'- defines {self._defines}')
      print(f'- linker options {self._ld_opts}')
      print(f'- used libs: {self._libs}')
      print(f'- lib path: {self._lib_path}')
      print(f'- inc path: {self._include}')
      print(f'- temp path: {self._temp_dir}')
    
    print(f'Ready for compile target "{ui.txt_green(self._target)}"')

  def compile(self):
    '''запуск сборки'''
    pass
