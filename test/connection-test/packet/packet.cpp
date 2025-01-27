#include "packet.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

namespace net {

void push_bytes(Bytes& dst, cp<byte> src, std::size_t src_sz) {
  ret_if (src_sz == 0);
  ret_if (!src);
  iferror(src_sz >= 512 * 1024 * 1024, "слишком много данных");
  dst.reserve(dst.size() + src_sz);
  dst.insert(dst.end(), src, src + src_sz);
}

void push_short_nickname(Bytes& dst, cr<utf32> nickname) {
  // лимитировать длину ника
  const u32_t nick_sz = std::min<u32_t>(nickname.size(), net::SHORT_NICKNAME_SZ);
  // отправить длину ника
  push_data(dst, nick_sz);
  // отправить символы ника
  if (nick_sz != 0) {
    push_bytes(dst, cptr2ptr<cp<byte>>(nickname.data()), nick_sz * sizeof(utf32::value_type));
  }
}

void push_str(Bytes& dst, cr<Str> str) {
  const u32_t sz = str.size();
  iferror(sz >= 512 * 1024 * 1024, "строка слишком большая");
  // отправить длину строки
  push_data(dst, sz);
  // отправить символы строки
  if (sz != 0) {
    push_bytes(dst, cptr2ptr<cp<byte>>(str.data()), sz * sizeof(Str::value_type));
  }
}

void push_utf32(Bytes& dst, cr<utf32> str) {
  const u32_t sz = str.size();
  iferror(sz >= 512 * 1024 * 1024, "строка слишком большая");
  // отправить длину строки
  push_data(dst, sz);
  // отправить символы строки
  if (sz != 0) {
    push_bytes(dst, cptr2ptr<cp<byte>>(str.data()), sz * sizeof(utf32::value_type));
  }
}

void read_bytes(cr<Bytes> src, byte* dst, std::size_t sz, std::size_t& pos) {
  iferror(!dst, "dst is null");
  iferror(src.empty(), "src is empty");
  iferror(pos >= src.size(), "невозможно прочитать больше данных, чем есть в потоке (" <<
    pos << "/" << src.size() << ")");
  std::memcpy(ptr2ptr<void*>(dst), cptr2ptr<cp<void>>(src.data() + pos), sz);
  pos += sz;
}

utf32 read_short_nickname(cr<Bytes> dst, std::size_t& pos) {
  // получить длину строки
  const u32_t sz = read_data<u32_t>(dst, pos);
  if (sz >= net::SHORT_NICKNAME_SZ)
    hpw_warning("размер никнейма превышает лимит\n");
  iferror(sz >= 512 * 1024 * 1024, "недопустимый размер строки");
  
  utf32 ret;
  // загрузить символы строки
  if (sz != 0) {
    ret.resize(sz);
    read_bytes(dst, ptr2ptr<byte*>(ret.data()), sz * sizeof(utf32::value_type), pos);
  }
  return ret;
}

Str read_str(cr<Bytes> dst, std::size_t& pos) {
  // получить длину строки
  const u32_t sz = read_data<u32_t>(dst, pos);
  iferror(sz >= 512 * 1024 * 1024, "недопустимый размер строки");
  
  Str ret;
  // загрузить символы строки
  if (sz != 0) {
    ret.resize(sz);
    read_bytes(dst, ptr2ptr<byte*>(ret.data()), sz * sizeof(Str::value_type), pos);
  }
  return ret;
}

utf32 read_utf32(cr<Bytes> dst, std::size_t& pos) {
  // получить длину строки
  const u32_t sz = read_data<u32_t>(dst, pos);
  iferror(sz >= 512 * 1024 * 1024, "недопустимый размер строки");
  
  utf32 ret;
  // загрузить символы строки
  if (sz != 0) {
    ret.resize(sz);
    read_bytes(dst, ptr2ptr<byte*>(ret.data()), sz * sizeof(utf32::value_type), pos);
  }
  return ret;
}

// генерирует контрольную сумму по данным
static inline Hash hasher(cp<byte> src, const std::size_t src_sz, Hash prev=0xFFFFu) {
  ret_if (!src, prev);
  ret_if (src_sz == 0, prev);
  iferror(src_sz >= 512 * 1024 * 1024, "недопустимый размер данных");

  constexpr Hash MUL = 255;
  constexpr Hash ADD = 1;
  cfor (i, src_sz) {
    prev *= MUL;
    prev += scast<Hash>(*src);
    ++src;
    prev += ADD;
  }
  
  return prev;
}

Hash get_packet_hash(cr<Packet> src) {
  cauto data_sz = src.bytes.size();
  iferror(data_sz < sizeof(Hash), "в пакете нету данных о чексумме");
  // в конце данных пакета не учитывать данные хэша
  return hasher(src.bytes.data(), data_sz - sizeof(Hash));;
}

Tag get_packet_tag(cr<Packet> src) {
  cauto data_sz = src.bytes.size();
  iferror(data_sz < sizeof(Tag), "метки нету в пакете");
  Tag ret = *(cptr2ptr<cp<Tag>>(src.bytes.data()));
  return ret;
}

} // net ns
