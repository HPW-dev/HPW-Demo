#pragma once
#include "util/file/file.hpp"

struct Audio;

Audio load_audio_from_memory(cr<File> file);
Audio load_audio(cr<Str> file_name);
