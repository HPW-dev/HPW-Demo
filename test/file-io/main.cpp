#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/str.hpp"
#include "util/file/file-io.hpp"
#include "util/path.hpp"
#include "util/error.hpp"
#include "util/vector-types.hpp"

struct Struct {
  std::uint8_t a {};
  std::uint8_t b {};
  float c {};
  std::uint8_t d {};
};

void write_test() {
  const Str fname = "delme.dat";
  constexpr std::uint8_t _u8 = 0x9Fu;
  constexpr std::int16_t _i16 = -30'000;
  constexpr std::uint32_t _u32 = 0xFFAA'FFBBu;
  constexpr std::uint64_t _u64 = 0xAABB'CCDD'EEFF'1234u;
  constexpr bool _bool1 = false;
  constexpr bool _bool2 = true;
  constexpr Struct _struct {
    .a = 23,
    .b = 27,
    .c = 29.123f,
    .d = 31
  };
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
  writer.write(cptr2ptr<cp<byte>>(_array.data()), _array.size() * sizeof(decltype(_array)::value_type));
  writer.close();

  iferror( !file_exists(fname), "file \"" + fname + "\" not created");
  hpw_log("writed: " + n2s(writer.writed()) + " bytes\n\n");
}

void read_test() {
  const Str fname = "delme.dat";
  hpw_log("test reading from \"" + fname + "\"\n");

  std::uint8_t _u8;
  std::int16_t _i16;
  std::uint32_t _u32;
  std::uint64_t _u64;
  bool _bool1;
  bool _bool2;
  Struct _struct;
  Vector<float> _array(3);

  File_reader reader;
  reader.open(fname);
  reader.read(_u8);
  reader.read(_i16);
  reader.read(_u32);
  reader.read(_u64);
  reader.read(_bool1);
  reader.read(_bool2);
  reader.read(_struct);
  reader.read(ptr2ptr<byte*>(_array.data()), _array.size() * sizeof(decltype(_array)::value_type));

  iferror(_u8 != 0x9Fu,                   "_u8 != 0x9Fu");
  iferror(_i16 != -30'000,                "_i16 != -30'000");
  iferror(_u32 != 0xFFAA'FFBBu,           "_u32 != 0xFFAA'FFBBu");
  iferror(_u64 != 0xAABB'CCDD'EEFF'1234u, "_u64 != 0xAABB'CCDD'EEFF'1234u");
  iferror(_bool1 != false,                "_bool1 != false");
  iferror(_bool2 != true,                 "_bool2 != true");
  iferror(_struct.a != 23,                "_struct.a != 23");
  iferror(_struct.b != 27,                "_struct.b != 27");
  iferror(_struct.c != 29.123f,           "_struct.c != 29.123f");
  iferror(_struct.d != 31,                "_struct.d != 31");
  iferror(_array.at(0) != 123.456f,       "_array.at(0) != 123.456f");
  iferror(_array.at(1) != 3.14f,          "_array.at(1) != 3.14f");
  iferror(_array.at(2) != 97.997f,        "_array.at(2) != 97.997f");

  reader.size();
  hpw_log("file size: " + n2s(reader.size()) + " bytes\n\n");
}

int main() {
  hpw_log("file I/O test\n\n");
  write_test();
  read_test();
}
