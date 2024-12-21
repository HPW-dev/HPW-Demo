#include "util/log.hpp"
#include "util/str.hpp"
#include "util/file/file-io.hpp"
#include "util/path.hpp"
#include "util/error.hpp"
#include "util/vector-types.hpp"

void write_test() {
  const Str fname = "delme.dat";
  constexpr std::uint8_t _u8 = 0x9Fu;
  constexpr std::int16_t _i16 = -30'000;
  constexpr std::uint32_t _u32 = 0xFFAA'FFBBu;
  constexpr std::uint64_t _u64 = 0xAABB'CCDD'EEFF'1234u;
  constexpr bool _bool1 = false;
  constexpr bool _bool2 = true;
  struct Struct {
    std::uint8_t a = 23;
    std::uint8_t b = 27;
    float c = 29.123f;
    std::uint8_t d = 31;
  };
  constexpr Struct _struct;
  Vector<float> _array(3);
  _array.at(0) = 123.456f;
  _array.at(1) = 3.14f;
  _array.at(2) = 97.997f;

  hpw_log("test writing to \"" + fname + "\"\n");
  File_writer writer(fname);
  writer.write(_u8);
  writer.write(_i16);
  writer.write(_u32);
  writer.write(_u64);
  writer.write(_bool1);
  writer.write(_bool2);
  writer.write(_struct);
  writer.write(cp<byte>(_array.data()), _array.size());
  writer.close();

  iferror( !file_exists(fname), "file \"" + fname + "\" not created");
}

void read_test() {
  const Str fname = "delme.dat";
  hpw_log("test reading from \"" + fname + "\"\n");
}

int main() {
  hpw_log("file I/O test\n");
  write_test();
  read_test();
}
