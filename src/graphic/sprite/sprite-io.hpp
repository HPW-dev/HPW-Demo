#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"

struct File;
class Sprite;

/// load from mem
void load(Sprite &dst, CN<File> file);
/// load from file
void load(Sprite &dst, CN<Str> name);
/// load from memory
void load(CN<File> file, Sprite &dst);
/// save to file
void save(CN<Sprite> src, Str file_name);
