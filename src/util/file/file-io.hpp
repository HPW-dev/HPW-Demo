#pragma once
#include "file.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/vector-types.hpp"

// записывает данные в файл
class File_writer final {
  struct Impl;
  Unique<Impl> _impl {};

public:
  File_writer();
  File_writer(cr<Str> fname);
  ~File_writer();
  void open(cr<Str> fname);
  void close();
  operator bool() const;
  void write(cp<byte> dst, const std::size_t sz);
  std::size_t writed() const; // сколько байтов уже записано

  template <class T>
  inline void write(cr<T> dst) {
    write(cptr2ptr<cp<byte>>(&dst), sizeof(T));
  }
};

// читает данные из файла
class File_reader final {
  struct Impl;
  Unique<Impl> _impl {};

public:
  File_reader();
  File_reader(cr<Str> fname);
  ~File_reader();
  void open(cr<Str> fname);
  void close();
  std::size_t size() const;
  operator bool() const;
  void read(byte* dst, const std::size_t sz);
  Bytes read_all();

  template <class T>
  inline void read(T& dst) {
    read(ptr2ptr<byte*>(&dst), sizeof(T));
  }
};

Bytes mem_from_file(cr<Str> fname);
void mem_to_file(cr<Bytes> src, cr<Str> fname);
File file_load(cr<Str> fname);
void file_save(cr<File> file);
