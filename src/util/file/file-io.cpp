#include "file-io.hpp"

struct File_writer::Impl {
  inline Impl() = default;

  inline Impl(cr<Str> fname) {}

  inline void open(cr<Str> fname) {}

  inline void close() {}

  inline std::size_t size() const {
    return 0; // TODO
  }

  inline operator bool() const {
    return false; // TODO
  }

  inline void write(cp<byte> dst, const std::size_t sz) {}
}; // File_writer::Impl 

struct File_reader::Impl {
  inline Impl() = default;

  inline Impl(cr<Str> fname) {}

  inline void open(cr<Str> fname) {}

  inline void close() {}

  inline std::size_t size() const {
    return 0; // TODO
  }

  inline operator bool() const {
    return false; // TODO
  }

  inline void read(byte* dst, const std::size_t sz) {}

  inline Bytes read_all() {
    return {}; // TODO
  }
}; // File_reader::Impl

File_writer::File_writer(): _impl{new_unique<Impl>()} {}
File_writer::File_writer(cr<Str> fname): _impl{new_unique<Impl>(fname)} {}
File_writer::~File_writer() {}
void File_writer::open(cr<Str> fname) { _impl->open(fname); }
void File_writer::close() { _impl->close(); }
File_writer::operator bool() const { return _impl->operator bool(); }
void File_writer::write(cp<byte> dst, const std::size_t sz) { _impl->write(dst, sz); }

File_reader::File_reader(): _impl{new_unique<Impl>()} {}
File_reader::File_reader(cr<Str> fname): _impl{new_unique<Impl>(fname)} {}
File_reader::~File_reader() {}
void File_reader::open(cr<Str> fname) { _impl->open(fname); }
void File_reader::close() { _impl->close(); }
File_reader::operator bool() const { return _impl->operator bool(); }
std::size_t File_reader::size() const { return _impl->size(); }
void File_reader::read(byte* dst, const std::size_t sz) { _impl->read(dst, sz); }
Bytes File_reader::read_all() { return _impl->read_all(); }
