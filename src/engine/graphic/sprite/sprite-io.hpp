#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"

struct File;
class Sprite;

// load from mem
void load(Sprite &dst, cr<File> file);
// load from file
void load(Sprite &dst, cr<Str> name);
// load from memory
void load(cr<File> file, Sprite &dst);
// save to file
void save(cr<Sprite> src, Str file_name);
