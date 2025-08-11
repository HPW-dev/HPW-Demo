#pragma once
template <class PIX_FMT> class Image_templ;
struct Pal8;
struct Rgb24;
using Image = Image_templ<Pal8>;
using Image_rgb24 = Image_templ<Rgb24>;
