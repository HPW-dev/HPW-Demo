extern "C" {
#include <zip/zip.h>
}
#include "archive.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/file/file-io.hpp"

Archive::Archive(Str fname) {
  conv_sep(fname);
  log_debug << "Archive: load \"" + fname + "\"";
  strm_buf = mem_from_file(fname);
  zip = zip_stream_open(cptr2ptr<Cstr>(strm_buf.data()),
    strm_buf.size(), 0, 'r');
  iferror( !zip, "Archive.c-tor: zip error (" << fname << ")");
  set_path(fname);
}

Archive::Archive(cr<File> file_mem) {
  cauto path = file_mem.get_path();
  log_debug << "Archive: load from memory \"" + path + "\"";
  strm_buf = file_mem.data;
  zip = zip_stream_open(cptr2ptr<Cstr>(strm_buf.data()),
    strm_buf.size(), 0, 'r');
  iferror( !zip, "Archive.c-tor: zip error (" << path << ")");
  set_path(path);
}

Archive::Archive(File&& file_mem) {
  cauto path = file_mem.get_path();
  log_debug << "Archive: load from memory \"" + path + "\"";
  strm_buf = std::move(file_mem.data);
  zip = zip_stream_open(cptr2ptr<Cstr>(strm_buf.data()),
    strm_buf.size(), 0, 'r');
  iferror( !zip, "Archive.move c-tor: zip error (" << path << ")");
  set_path(path);
}

Archive::~Archive() { zip_close(zip); }

File Archive::get_file(Str fname) const {
  replace_all(fname, "\\", "/");
  replace_all(fname, "//", "/");
  delete_all(fname, "./");
  log_debug << "Archive.get_file:\"" + fname + "\"";
  _zip_check(zip_entry_open(zip, fname.c_str()),
    "Archive.get_file: zip_entry_open \"" + fname + "\"");

  auto out_size = zip_entry_size(zip);
  if (out_size < 1) {
    log_warning << "Archive.get_file: file \"" + fname + "\" is empty";
    return {};
  }
  
  File ret (Bytes(out_size), fname);
  _zip_check(zip_entry_noallocread(zip, ret.data.data(), ret.data.size()),
    "Archive.get_file: zip_entry_noallocread");
  _zip_check(zip_entry_close(zip), "Archive.get_file: zip_entry_close");
  return ret;
} // get_file

Strs Archive::get_all_names(bool with_folders) const {
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
    const Str path = zip_entry_name(zip);
    if (!with_folders) {
      if (!path.empty() && path.back() != '/')
        ret.push_back(path);
    } else {
      ret.push_back(path);
    }
    _zip_check(zip_entry_close(zip), "Archive.get_all_names: zip_entry_close");
    ++idx;
  }
  return ret;
} // get_all_names

void Archive::_zip_check(int errcode, cr<Str> fname) const {
  iferror(errcode < 0, fname << ": " << zip_strerror(errcode));
}
