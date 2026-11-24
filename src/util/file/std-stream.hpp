#pragma once
#include <fstream>

#include "util/macro.hpp"
#include "util/str.hpp"


// безопасно получить std::ifstream с учётом кирилицы в путях
std::ifstream open_ifstream(cr<Str> path, std::ios::openmode mode = std::ios::in);
// безопасно получить std::ofstream с учётом кирилицы в путях
std::ofstream open_ofstream(cr<Str> path, std::ios::openmode mode = std::ios::out);
