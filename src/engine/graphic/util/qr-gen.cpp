#include "pch.hpp"
#include "qr-gen.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/util/resize.hpp"
#include "thirdparty/include/Nayuki/QR-Code-generator/qrcodegen.hpp"

Image qr_gen(cr<Str> text) {
  cauto err_cor_lvl = qrcodegen::QrCode::Ecc::LOW; // слабая коррекция ошибок
  cauto qr = qrcodegen::QrCode::encodeText(text.c_str(), err_cor_lvl);
  cauto qr_sz = qr.getSize();
  cauto border = 2;

  Image res(qr_sz + border * 2, qr_sz + border * 2);
  for (int y = -border; y < qr_sz + border; y++) {
    for (int x = -border; x < qr_sz + border; x++) {
      Pal8 c = qr.getModule(x, y) ? Pal8::black : Pal8::white;
      res.set(x + border, y + border, c);
    }
  }

  zoom_x8(res);
  return res;
}
