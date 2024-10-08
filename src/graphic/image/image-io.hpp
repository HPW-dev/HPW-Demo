#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/file/file.hpp"

class Image;

// load from mem
void load(Image& dst, cr<Bytes> mem);
// load from file
void load(Image& dst, cr<Str> name);
// save to file
void save(cr<Image> src, Str name);
