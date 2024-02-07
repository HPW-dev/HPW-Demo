#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/file/file.hpp"

class Image;

/// load from mem
void load(Image& dst, CN<Bytes> mem);
/// load from file
void load(Image& dst, CN<Str> name);
/// save to file
void save(CN<Image> src, Str name);
