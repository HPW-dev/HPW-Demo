#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
  
// получить имя файла (без расширения)
Str get_filename(cr<Str> str);
// получить расширение файла
Str get_fileext(cr<Str> str);
// получить путь до файла. перед вызовом переконвертируй сепараторы
Str get_filedir(cr<Str> str);
// получить имена файлов в директории
Strs files_in_dir(cr<Str> path);
// создаёт папку, если её нет
void make_dir_if_not_exist(cr<Str> dir);
// текущая директория запуска (в конце/)
Str launch_dir_from_argv0(Cstr str);
