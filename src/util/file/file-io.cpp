#include <fstream>
#include <filesystem>
#include "file.hpp"
#include "file-io.hpp"
#include "util/error.hpp"

struct File_writer::Impl {
  std::ofstream _file {};
  std::size_t _total_writed {};

  inline Impl() = default;
  inline ~Impl() { close(); };

  inline Impl(cr<Str> fname) { open(fname); }

  inline void open(cr<Str> fname) {
    iferror(fname.empty(), "file name is empty");

    try {
      cauto path = std::filesystem::path(fname);
      _file.open(path, std::ios_base::binary);

      if (!operator bool())
        throw hpw::Error{};
    } catch (...) {
      error("error while opening file \"" + fname + "\"");
    }

    _total_writed = 0;
  }

  inline void close() { _file.close(); }

  inline operator bool() const { return _file && _file.is_open(); }

  inline void write(cp<byte> dst, const std::size_t sz) {
    return_if(sz == 0);
    iferror(!operator bool(), "bad file for writing data");
    iferror(!dst, "input data (dst) is null");
    _file.write(cptr2ptr<cp<char>>(dst), sz);
    _total_writed += sz;
  }

  inline std::size_t writed() const { return _total_writed; }
}; // File_writer::Impl 

struct File_reader::Impl {
  std::ifstream _file {};
  std::size_t _sz {};
  std::size_t _readed {};

  inline Impl() = default;
  inline ~Impl() { close(); };

  inline Impl(cr<Str> fname) { open(fname); }

  inline void open(cr<Str> fname) {
    iferror(fname.empty(), "file name is empty");

    try {
      cauto path = std::filesystem::path(fname);
      _file.open(path, std::ios_base::binary);

      if (!operator bool())
        throw hpw::Error{};
      
      // узнать размер файла в байтах
      _file.seekg(0, std::ios::end);
      _sz = _file.tellg();
      _file.seekg(0, std::ios::beg);
    } catch (...) {
      error("error while opening file \"" + fname + "\"");
    }

    _readed = 0;
  }

  inline void close() { _file.close(); }

  inline std::size_t size() const { return _sz; }

  inline operator bool() const { return _file && _file.is_open(); }

  inline void read(byte* dst, const std::size_t sz) {
    iferror(!operator bool(), "bad file for reading data");
    iferror(!dst, "bad output data (dst is null)");
    cauto readed = _readed + sz;
    iferror(readed > _sz, "попытка прочитать данных из файла больше, чем есть ("
      << readed << "/" << _sz << ")");
    _file.read(ptr2ptr<char*>(dst), sz);
    _readed = readed;
  }

  inline Bytes read_all() {
    iferror(!operator bool(), "bad file for reading data");
    _file.seekg(0, std::ios::beg);
    Bytes ret(_sz);
    _file.read(ptr2ptr<char*>(ret.data()), _sz);
    _readed = _sz;
    return ret;
  }
}; // File_reader::Impl

File_writer::File_writer(): _impl{new_unique<Impl>()} {}
File_writer::File_writer(cr<Str> fname): _impl{new_unique<Impl>(fname)} {}
File_writer::~File_writer() {}
void File_writer::open(cr<Str> fname) { _impl->open(fname); }
void File_writer::close() { _impl->close(); }
File_writer::operator bool() const { return _impl->operator bool(); }
void File_writer::write(cp<byte> dst, const std::size_t sz) { _impl->write(dst, sz); }
std::size_t File_writer::writed() const { return _impl->writed(); }

File_reader::File_reader(): _impl{new_unique<Impl>()} {}
File_reader::File_reader(cr<Str> fname): _impl{new_unique<Impl>(fname)} {}
File_reader::~File_reader() {}
void File_reader::open(cr<Str> fname) { _impl->open(fname); }
void File_reader::close() { _impl->close(); }
File_reader::operator bool() const { return _impl->operator bool(); }
std::size_t File_reader::size() const { return _impl->size(); }
void File_reader::read(byte* dst, const std::size_t sz) { _impl->read(dst, sz); }
Bytes File_reader::read_all() { return _impl->read_all(); }

File file_load(cr<Str> fname) {
  File_reader reader(fname);
  return File(reader.read_all(), fname);
}

void file_save(cr<File> file) {
  File_writer writer(file.get_path());
  writer.write(file.data.data(), file.data.size());
}

Bytes mem_from_file(cr<Str> fname) {
  File_reader reader(fname);
  return reader.read_all();
}

void mem_to_file(cr<Bytes> src, cr<Str> fname) {
  File_writer writer(fname);
  writer.write(src.data(), src.size());
}
