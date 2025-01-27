#pragma once
#include "util/net/net.hpp"
#include "util/macro.hpp"
#include "util/unicode.hpp"

namespace net {

// интерфейс для генераци пакетов и чтения данных из потока байтов
class Pck_gen_base {
public:
  Pck_gen_base() = default;
  virtual ~Pck_gen_base() = default;
  virtual void from_packet(cr<Packet> src) = 0;
  virtual Packet to_packet() const = 0;
  virtual Tag tag() const = 0;
};

// записать байты в поток
void push_bytes(Bytes& dst, cp<byte> src, std::size_t src_sz);
// записать в поток укороченное имя игрока
void push_short_nickname(Bytes& dst, cr<utf32> nickname);
// отправить в поток UTF-8 строку
void push_str(Bytes& dst, cr<Str> str);
// отправить в поток UTF-32 строку
void push_utf32(Bytes& dst, cr<utf32> str);
// прочитать байты из потока
void read_bytes(cr<Bytes> src, byte* dst, std::size_t sz, std::size_t& pos);
// прочитать из потока укороченное имя игрока
utf32 read_short_nickname(cr<Bytes> dst, std::size_t& pos);
// прочитать из потока UTF-8 строку
Str read_str(cr<Bytes> dst, std::size_t& pos);
// прочитать из потока UTF-32 строку
utf32 read_utf32(cr<Bytes> dst, std::size_t& pos);

// записать конкретный тип в поток
template <class T>
void push_data(Bytes& dst, cr<T> src) {
  cauto src_bytes = cptr2ptr<cp<byte>>(&src);
  push_bytes(dst, src_bytes, sizeof(T));
}

template <class T>
T read_data(cr<Bytes> src, std::size_t& pos) {
  T ret;
  read_bytes(src, ptr2ptr<byte*>(&ret), sizeof(T), pos);
  return ret;
}

} // net ns
