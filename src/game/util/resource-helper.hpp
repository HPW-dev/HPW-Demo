#pragma once
#include <cstddef>
#include "util/str.hpp"

struct File;

// сохраняет все текстуры в папку в виде атласной текстуры
void save_all_sprites(const Str& save_dir, const int MX=512, const int MY=512);
std::size_t sizeof_all_sprites(); // размер всех спрайтов в банке
void load_resources(); // загрузить все иговые ресурсы
File load_res(const Str& name); // загрузить ресурс игры
Strs get_all_res_names(const bool with_folders = true); // получить все имена ресурсов