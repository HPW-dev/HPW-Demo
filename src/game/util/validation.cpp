#include <sstream>
#include "validation.hpp"
#include "game/core/common.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/macro.hpp"
#include "util/file/file.hpp"
#include "hash_sha256/hash_sha256.h"

inline static Str calc_sum(cp<void> data, std::size_t sz) {
  hash_sha256 hash;
  hash.sha256_init();
  hash.sha256_update(scast<cp<uint8_t>>(data), sz);
  auto hash_ret = hash.sha256_final();
        
  std::stringstream ss;
  for (auto val: hash_ret)
    ss << std::hex << int(val);
  return str_toupper(ss.str());
}

void init_validation_info() {
  // EXE
  #ifdef WINDOWS
    Str path = hpw::cur_dir + "HPW.exe";
  #else
    Str path = hpw::cur_dir + "HPW";
  #endif
  auto mem = mem_from_file(path);
  hpw::exe_sha256 = calc_sum( scast<cp<void>>(mem.data()), mem.size() );

  // DATA
  path = hpw::cur_dir + hpw::data_path;
  mem = mem_from_file(path);
  hpw::data_sha256 = calc_sum( scast<cp<void>>(mem.data()), mem.size() );

  hpw_log("game executable SHA256: " + hpw::exe_sha256 + "\n");
  hpw_log("game data.zip SHA256: " + hpw::data_sha256 + "\n");
}
