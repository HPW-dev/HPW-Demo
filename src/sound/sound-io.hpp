#pragma once
#include "sound-buffer.hpp"

struct File;

namespace sound {
// декодирование файлов в аудио-буффер для воспроизведения
Buffer file_to_buffer(cr<File> file);
} // sound ns
