#pragma once

namespace hpw {
  inline bool replay_read_mode {false}; /// воспроизволит реплей, иначе - записывает
  inline bool enable_replay {true}; /// включает запись реплея
  inline Str cur_replay_file_name {}; /// имя файла реплея, для проигрывания
}
