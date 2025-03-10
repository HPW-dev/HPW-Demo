#include "sound-io.hpp"
#include "util/file/file.hpp"
#include "util/log.hpp"

namespace sound {

Buffer file_to_buffer(cr<File> file) {
  log_debug << "convert file \"" << file.get_path() << "\" to audio-buffer";
  log_warning << "TODO need impl"; // TODO
  return {}; // TODO
}

} // sound ns
