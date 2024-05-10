#pragma once
#include "util/file/file.hpp"

struct Audio;

Audio load_audio_from_archive(CN<Str> file_name);
Audio load_audio(CN<Str> file_name);
