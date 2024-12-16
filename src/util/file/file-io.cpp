#include "file-io.hpp"

struct File_writer::Impl {
  inline Impl() = default;
};

File_writer::File_writer(): _impl{new_unique<Impl>()} {}
File_writer(cr<Str> fname): _impl{new_unique<Impl>(fname)} {}
File_writer::~File_writer() {}

void open(cr<Str> fname);
void close();
bool operator bool() const;
void write(cp<byte> dst, const std::size_t sz);
