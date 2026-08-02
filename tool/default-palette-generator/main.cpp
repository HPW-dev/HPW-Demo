#include <cassert>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <random>

using real = double;

struct srgb {
    real r {};
    real g {};
    real b {};

    inline operator std::string () const {
        std::string ret; 
        ret += std::to_string(std::clamp<int>(r * 255, 0, 255)) + " ";
        ret += std::to_string(std::clamp<int>(g * 255, 0, 255)) + " ";
        ret += std::to_string(std::clamp<int>(b * 255, 0, 255)) + " "; 
        return ret;
    }
};

inline static real hue_to_rgb(real p, real q, real t) {
    if (t < 0.0) t += 1.0;
    if (t > 1.0) t -= 1.0;
    if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0/2.0) return q;
    if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

struct hsl {
  real h {};
  real s {};
  real l {};

  hsl() = default;

  inline srgb to_srgb() const {
        srgb ret;
        real _h = h;
        real _s = s;
        real _l = l;
        // Convert H to a 0-1 range if it's in degrees
        if (_h > 360.0) _h = std::fmod(_h, 360.0);
        _h /= 360.0;

        if (_s == 0.0) {
            // Gray color, R=G=B=L
            ret.r = ret.g = ret.b = _l;
        } else {
            float q;
            if (_l < 0.5) {
                q = _l * (1.0 + _s);
            } else {
                q = _l + _s - _l * _s;
            }
            float p = 2.0 * _l - q;
            ret.r = hue_to_rgb(p, q, _h + 1.0/3.0);
            ret.g = hue_to_rgb(p, q, _h);
            ret.b = hue_to_rgb(p, q, _h - 1.0/3.0);
        }
        return ret;
    }

    inline hsl(const srgb& color) {
        const auto r = color.r;
        const auto g = color.g;
        const auto b = color.b;

        // Находим минимальное и максимальное значения среди R, G, B
        real min = std::min({r, g, b});
        real max = std::max({r, g, b});
        
        // Вычисляем светлоту (L)
        l = (min + max) / 2.0;

        // Если min == max, то цвет — оттенок серого (S = 0)
        if (min == max) {
            h = 0;
            s = 0;
            return;
        }

        // Вычисляем насыщенность (S)
        real delta = max - min;
        s = (l < 0.5)
            ? (delta / (max + min))
            : (delta / (2.0 - max - min));

        // Вычисляем оттенок (H)
        if (r == max) {
            h = (g - b) / delta;  // Между жёлтым и пурпурным
        } else if (g == max) {
            h = 2.0 + (b - r) / delta;  // Между зелёным и жёлтым
        } else {
            h = 4.0 + (r - g) / delta;  // Между пурпурным и зелёным
        }

        h *= 60.0;  // Переводим в градусы
        if (h < 0.0)
            h += 360.0;  // Корректируем отрицательные значения
    }
};

// Функция для перевода линейного значения [0, 1] в sRGB [0, 255]
real linear_to_srgb(real linear) {
    // Стандарт sRGB: пороговое значение 0.0031308
    if (linear <= 0.0031308)
        return linear * 12.92;

    // Коэффициенты стандарта: a=1.055, gamma=2.4
    return 1.055 * std::pow(linear, 1.0 / 2.4) - 0.055;
}

real srgb_to_linear(real s) {
    if (s <= 0.04045)
        return s / 12.92;

    return std::pow((s + 0.055) / 1.055, 2.4);
}

struct oklab {
    real l {};
    real a {};
    real b {};
};

struct oklch {
    real l {};
    real c {};
    real h {};
};

// sRGB → OKLab
oklab srgb_to_oklab(srgb rgb) {
    rgb.r = srgb_to_linear(rgb.r);
    rgb.g = srgb_to_linear(rgb.g);
    rgb.b = srgb_to_linear(rgb.b);

    // 1. Переход в конусное пространство LMS
    double l_cone = 0.4122214708 * rgb.r + 0.5363325363 * rgb.g + 0.0514459929 * rgb.b;
    double m_cone = 0.2119034982 * rgb.r + 0.6806995451 * rgb.g + 0.1073969566 * rgb.b;
    double s_cone = 0.0883024619 * rgb.r + 0.2817188376 * rgb.g + 0.6299787005 * rgb.b;

    // 2. Нелинейное преобразование (кубический корень)
    double l_ = std::cbrt(l_cone);
    double m_ = std::cbrt(m_cone);
    double s_ = std::cbrt(s_cone);

    // 3. Переход в компоненты Oklab (L - яркость, a/b - хроматичность)
    return oklab {
        .l = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_,
        .a = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_,
        .b = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_
    };
}

// OKLab → sRGB
srgb oklab_to_srgb(const oklab& lab) {
    // 1. Обратный переход в пространство LMS
    double l_p = lab.l + 0.3963377774 * lab.a + 0.2158037573 * lab.b;
    double m_p = lab.l - 0.1055613458 * lab.a - 0.0638541728 * lab.b;
    double s_p = lab.l - 0.0894841775 * lab.a - 1.2914855480 * lab.b;

    // 2. Обратное нелинейное преобразование (возведение в куб)
    double l = l_p * l_p * l_p;
    double m = m_p * m_p * m_p;
    double s = s_p * s_p * s_p;

    // 3. Обратно в Linear sRGB
    srgb ret {
        .r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
        .g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
        .b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s
    };

    ret.r = linear_to_srgb(ret.r);
    ret.g = linear_to_srgb(ret.g);
    ret.b = linear_to_srgb(ret.b);
    return ret;
}

srgb oklab_lerp(const srgb a, const srgb b, real frac) {
    auto ok_a = srgb_to_oklab(a);
    const auto ok_b = srgb_to_oklab(b);
    ok_a.l = std::lerp(ok_a.l, ok_b.l, frac);
    ok_a.a = std::lerp(ok_a.a, ok_b.a, frac);
    ok_a.b = std::lerp(ok_a.b, ok_b.b, frac);
    return oklab_to_srgb(ok_a);
}

srgb color_luma(real l) {
    assert(l >= 0 && l <= 1);

    static const std::vector<srgb> colors {
        /*
        srgb {.r=0,.g=0,.b=0},
        srgb {.r=0,.g=0,.b=1},
        srgb {.r=1,.g=0,.b=0},
        srgb {.r=1,.g=0,.b=1},
        srgb {.r=0,.g=1,.b=1},
        srgb {.r=1,.g=1,.b=0},
        srgb {.r=1,.g=1,.b=1},
        */

        srgb {.r=0,.g=0,.b=0},
        //srgb {.r=1./3.,.g=2./3.,.b=2./3.},
        srgb {.r=1,.g=0,.b=0.5},
        srgb {.r=1,.g=1,.b=1},
    };

    const auto real_idx = (colors.size() - 1) * l;
    const size_t a = std::floor(real_idx);
    const size_t b = std::min(a + 1, colors.size()-1);
    const auto frac = real_idx - a;
    const auto ac = colors[a];
    const auto bc = colors[b];

    #if 1
        return srgb {
            .r = std::lerp(ac.r, bc.r, frac),
            .g = std::lerp(ac.g, bc.g, frac),
            .b = std::lerp(ac.b, bc.b, frac)
        };
    #else
        return oklab_lerp(ac, bc, frac);
    #endif
}

srgb oklch_to_srgb(const oklch& src) {
    // 1. LCH to Lab
    // h задается в градусах, переводим в радианы
    real L = src.l;
    real a = src.c * std::cos(src.h * (std::numbers::pi / 180.0));
    real b = src.c * std::sin(src.h * (std::numbers::pi / 180.0));

    // 2. Lab to LMS (переход в цветовое пространство конусов)
    real l_ = L + 0.3963377774 * a + 0.2158037573 * b;
    real m_ = L - 0.1055613458 * a - 0.0638541728 * b;
    real s_ = L - 0.0894841775 * a - 1.2914855480 * b;

    // Возводим в куб для перехода в линейное пространство
    real l = l_ * l_ * l_;
    real m = m_ * m_ * m_;
    real s = s_ * s_ * s_;

    // 3. LMS to Linear RGB (матрица трансформации)
    real r_lin = +4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s;
    real g_lin = -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s;
    real b_lin = -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s;

    // 4. Linear RGB to sRGB (гамма-коррекция и ограничение 0..1)
    /* sRGB result;
    
    // Функция clamp(x, 0, 1) важна, так как OKLCH может описывать цвета вне охвата sRGB
    real r = std::clamp<real>(linear_to_srgb(r_lin), 0, 1);
    real g = std::clamp<real>(linear_to_srgb(g_lin), 0, 1);
    real b = std::clamp<real>(linear_to_srgb(b_lin), 0, 1);*/

    return srgb{
        .r = linear_to_srgb(r_lin),
        .g = linear_to_srgb(g_lin),
        .b = linear_to_srgb(b_lin)
    };
}

inline static double distance(double x0, double y0, double x1, double y1) {
  return std::sqrt(std::pow(x0-x1, 2) + std::pow(y0-y1, 2));
}

constexpr auto pow2(const auto x) { return x*x; }

inline static real to_luma(const hsl& src) {
    auto c = src.to_srgb();
    return
        c.r * 0.298839 +
        c.g * 0.586811 +
        c.b * 0.114350;
}

real blend_hue(real a, real b, real t) {
    auto dh = b - a;
    if (dh > 180.0) dh -= 360.0;
    else if (dh < -180.0) dh += 360.0;
    return a + t * dh;
}

oklch lerp(const oklch& a, const oklch& b, real t) {
    oklch ret;
    ret.l = std::lerp(a.l, b.l, t);
    ret.c = std::lerp(a.c, b.c, t);
    ret.h = blend_hue(a.h, b.h, t);
    return ret;
}

int main() {
    constexpr int W = 256;
    constexpr int H = 11;
    constexpr int RED_SZ = 32;
    constexpr int GRAY_SZ = 255-32-1;
    constexpr int WHITE_START = 255;
    constexpr int RED_START = GRAY_SZ+1;

    std::ofstream file("result.ppm");
    file << "P3\n";
    file << W << " " << H << "\n";
    file << "255\n";

    for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x) {
        #if 1
        oklch col;
        if (x < RED_START) {
            col.l = x / real(GRAY_SZ);
            col.h = 0;
            col.c = 0;
        } else if (x >= RED_START && x < WHITE_START) {
            col.l = ((x - RED_START) / real(RED_SZ)) * 0.628;
            col.h = 29.23;
            col.c = 0.2577;
        } else {
            col.l = 1;
            col.c = 0;
            col.h = 0;
        }

        file << std::string(oklch_to_srgb(col));
        #endif

        #if 0
        hsl col;
        if (x < RED_START) {
            col.h = 0;
            col.s = 0;
            col.l = x / real(GRAY_SZ);
        } else if (x >= RED_START && x < WHITE_START) {
            col.h = 0;
            col.s = 1;
            col.l = ((x - RED_START) / real(RED_SZ)) * 0.5;
        } else {
            col.h = 0;
            col.s = 0;
            col.l = 1;
        }
        file << std::string(col.to_srgb());
        #endif

        #if 0
        oklab col;
        if (x < RED_START) {
            col.l = x / real(GRAY_SZ);
            col.a = 0;
            col.b = 0;
        } else if (x >= RED_START && x < WHITE_START) {
            auto ratio = ((x - RED_START) / real(RED_SZ));
            col.l = ratio * 0.628;
            col.a = ratio * 0.225;
            col.b = ratio * 0.126;
        } else {
            col.l = 1;
            col.a = 0;
            col.b = 0;
        }
        file << std::string(oklab_to_srgb(col));
        #endif

        #if 0
        srgb col;
        if (x < RED_START) {
            auto l = x / real(GRAY_SZ);
            col.r = l;
            col.g = l;
            col.b = l;
        } else if (x >= RED_START && x < WHITE_START) {
            col.r = (x - RED_START) / real(RED_SZ);
            col.g = col.b = 0;
        } else {
            col.r = col.g = col.b = 1;
        }
        file << std::string(col);
        #endif

        #if 0
        int r, g, b;
        if (x < RED_START) {
            auto l = x / real(GRAY_SZ);
            r = l * 255;
            g = l * 255;
            b = l * 255;
        } else if (x >= RED_START && x < WHITE_START) {
            r = (x - RED_START) / real(RED_SZ) * 255;
            g = b = 0;
        } else {
            r = g = b = 255;
        }
        file << r << " " << g << " " << b << " ";
        #endif
    }

    file << "\n";
}
