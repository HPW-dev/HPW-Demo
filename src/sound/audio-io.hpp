#pragma once
#include "util/file/file.hpp"

struct Audio;

Audio load_audio(CN<Bytes> mem);
Audio load_audio(CN<Str> fname);
