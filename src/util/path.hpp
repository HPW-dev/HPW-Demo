#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
  
// получить имя файла (без расширения)
Str get_filename(CN<Str> str);
// получить расширение файла
Str get_fileext(CN<Str> str);
// получить путь до файла. перед вызовом переконвертируй сепараторы
Str get_filedir(CN<Str> str);
// получить имена файлов в директории
Strs files_in_dir(CN<Str> path);
// создаёт папку, если её нет
void make_dir_if_not_exist(CN<Str> dir);
// текущая директория запуска (в конце/)
Str launch_dir_from_argv0(Cstr str);
