### Для Windows

**Минимальный поддерживаемый стандарт для сборки: C++23**.

Я пользуюсь компилятором MinGW-W64 i686-ucrt-mcf-dwarf 13.2.0 из [WinLibs](https://winlibs.com/).

Все зависимости уже собраны в .dll/.a файлы, если хотите собрать их самостоятельно, то [см. список используемых библиотек](../version.txt).

В папке [thirdparty](../thirdparty/) уже лежат собранные бинарники, вытащите их из архивов dlls.zip и libs.zip - lib папку оставьте в thirdparty, а .dll файлы закиньте в [build](../build/).

### Для Linux

При сборке в Линуксе, зависимости докачайте самостоятельно через ваш пакетный менеджер.

**Требуемые пакеты: GCC/Clang, GLEW, GLFW3, OpenAL-soft, YAML-cpp.**

### Сборка игры

Для сборки потребуется [система сборки SCons](https://scons.org/) и Python интерпретатор.

Команда на скачиваение через pip: ```py -m pip install --user scons```

Команды на сборку игры:
```
git clone --depth=1 https://github.com/HPW-dev/HPW-Demo
cd HPW-Demo
py "script/build-game-release.py"
```
