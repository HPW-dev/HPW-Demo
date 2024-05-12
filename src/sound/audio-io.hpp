#pragma once
#include "util/file/file.hpp"

struct Audio;

Audio load_audio_from_memory(CN<File> file);
Audio load_audio(CN<Str> file_name);
