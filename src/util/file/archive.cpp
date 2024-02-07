extern "C" {
#include <zip/zip.h>
}
#include <fstream>
#include "archive.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/file.hpp"

File::File(CN<Bytes> _data, CN<Str> _path)
: Resource {_path}
, data {_data}
{}

Archive::Archive(Str fname) {
  conv_sep(fname);
  detailed_log("Archive: load \"" << fname << "\"\n");
  strm_buf = mem_from_file(fname);
  zip = zip_stream_open(cptr2ptr<Cstr>(strm_buf.data()),
    strm_buf.size(), 0, 'r');
  iferror( !zip, "Archive.c-tor: zip error");
}

Archive::~Archive() { zip_close(zip); }

File Archive::get_file(Str fname) const {
  replace_all(fname, "\\", "/");
  replace_all(fname, "//", "/");
  delete_all(fname, "./");
  detailed_log("Archive.get_file:\"" << fname << "\"\n");
  _zip_check(zip_entry_open(zip, fname.c_str()),
    "Archive.get_file: zip_entry_open \"" + fname + "\"");
  auto out_size = zip_entry_size(zip);
  if (out_size < 1) {
    hpw_log("Archive.get_file: file \"" + fname + "\" is empty\n");
    return {};
  }
  File ret (Bytes(out_size), fname);
  _zip_check(zip_entry_noallocread(zip, ret.data.data(), ret.data.size()),
    "Archive.get_file: zip_entry_noallocread");
  _zip_check(zip_entry_close(zip), "Archive.get_file: zip_entry_close");
  return ret;
} // get_file

Strs Archive::get_all_names() const {
  Strs ret;
  int idx = 0;
  int errcode;
  while (true) {
    errcode = zip_entry_openbyindex(zip, idx); 
    if (errcode == ZIP_EINVIDX) {
      _zip_check(zip_entry_close(zip), "Archive.get_all_names: zip_entry_close");
      break;
    }
    _zip_check(errcode, "Archive.get_all_names: zip_entry_openbyindex");
    ret.push_back(zip_entry_name(zip));
    _zip_check(zip_entry_close(zip), "Archive.get_all_names: zip_entry_close");
    ++idx;
  }
  return ret;
} // get_all_names

void Archive::_zip_check(int errcode, CN<Str> fname) const {
  iferror(errcode < 0, fname << ":" << zip_strerror(errcode));
}
