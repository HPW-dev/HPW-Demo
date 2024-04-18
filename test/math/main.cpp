#include <cmath>
#include "util/math/mat.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"

#ifndef DEBUG
#define DEBUG
#include "util/log.hpp"
#endif

inline constexpr uint32_t to_u32(real r) {
  return *scast<uint32_t*>(std::addressof(r));
}

struct Point {
  real x {};
  real y {};
};

Point simple_physics() {
  Point pos {0.12445, 1244.55311};
  cfor (_, 240*60*30) {
    pos.x += 0.00124;
    pos.y += -0.00007;
    auto len = std::sqrt(pow2(pos.x) + pow2(pos.y));
    pos.x = safe_div(pos.x, len);
    pos.y = safe_div(pos.y, len);
  }
  return pos;
}

Point simple_physics_2() {
  Point pos {0.12445, 1244.55311};
  cfor (_, 240*60*30) {
    pos.x += -0.002007;
    pos.y += 0.00009;
  }
  return pos;
}

int main() {
  try {
    hpw_assert(sizeof(real) == 4);

    real a = 0.97997;
    real b = 3.000095;
    hpw_log("a: " << n2s(a, 8) << ", " << std::hex << to_u32(a) << std::endl);
    hpw_log("b: " << n2s(b, 8) << ", " << std::hex << to_u32(b) << std::endl);

    auto mul_ab = a * b;
    hpw_log("a * b: " << n2s(mul_ab, 8) << ", " << std::hex << to_u32(mul_ab) << std::endl);

    auto pow2_a = pow2(a);
    auto pow2_b = pow2(b);
    hpw_log("pow2 a: " << n2s(pow2_a, 8) << ", " << std::hex << to_u32(pow2_a) << std::endl);
    hpw_log("pow2 b: " << n2s(pow2_b, 8) << ", " << std::hex << to_u32(pow2_b) << std::endl);

    auto pow_ab = std::pow(a, b);
    hpw_log("pow(a, b): " << n2s(pow_ab, 8) << ", " << std::hex << to_u32(pow_ab) << std::endl);

    auto sqrt_a = std::sqrt(a);
    auto sqrt_b = std::sqrt(b);
    hpw_log("sqrt a: " << n2s(sqrt_a, 8) << ", " << std::hex << to_u32(sqrt_a) << std::endl);
    hpw_log("sqrt b: " << n2s(sqrt_b, 8) << ", " << std::hex << to_u32(sqrt_b) << std::endl);

    const real pi = PI;
    hpw_log("pi: " << n2s(pi, 8) << ", " << std::hex << to_u32(pi) << std::endl);

    real sum_x3 = a;
    sum_x3 += a;
    sum_x3 += a;
    hpw_log("sum a x3: " << n2s(sum_x3, 8) << ", " << std::hex << to_u32(sum_x3) << std::endl);

    real mul_x3 = a;
    mul_x3 *= a;
    mul_x3 *= a;
    hpw_log("mul a x3: " << n2s(mul_x3, 8) << ", " << std::hex << to_u32(mul_x3) << std::endl);

    real sin_a = std::sin(a);
    real sin_b = std::sin(b);
    real cos_a = std::cos(a);
    real cos_b = std::cos(b);
    real atan2_ab = std::atan2(a, b);
    hpw_log("sin a: " << n2s(sin_a, 8) << ", " << std::hex << to_u32(sin_a) << std::endl);
    hpw_log("sin b: " << n2s(sin_b, 8) << ", " << std::hex << to_u32(sin_b) << std::endl);
    hpw_log("cos a: " << n2s(cos_a, 8) << ", " << std::hex << to_u32(cos_a) << std::endl);
    hpw_log("cos a: " << n2s(cos_b, 8) << ", " << std::hex << to_u32(cos_b) << std::endl);
    hpw_log("atan2(a, b): " << n2s(atan2_ab, 8) << ", " << std::hex << to_u32(atan2_ab) << std::endl);
    
    real c = 0.12345678;
    real d = 54321.0123;
    hpw_log("c: " << n2s(c, 8) << ", " << std::hex << to_u32(c) << std::endl);
    hpw_log("d: " << n2s(d, 8) << ", " << std::hex << to_u32(d) << std::endl);

    auto sum_cd = c + d;
    auto sub_cd = c - d;
    auto mul_cd = c * d;
    auto div_cd = c / d;
    auto sin_d = std::sin(d);
    auto sqrt_d = std::sqrt(d);
    hpw_log("c + d: " << n2s(sum_cd, 8) << ", " << std::hex << to_u32(sum_cd) << std::endl);
    hpw_log("c - d: " << n2s(sub_cd, 8) << ", " << std::hex << to_u32(sub_cd) << std::endl);
    hpw_log("c * d: " << n2s(mul_cd, 8) << ", " << std::hex << to_u32(mul_cd) << std::endl);
    hpw_log("c / d: " << n2s(div_cd, 8) << ", " << std::hex << to_u32(div_cd) << std::endl);
    hpw_log("sin(d): " << n2s(sin_d, 8) << ", " << std::hex << to_u32(sin_d) << std::endl);
    hpw_log("sqrt_d(d): " << n2s(sqrt_d, 8) << ", " << std::hex << to_u32(sqrt_d) << std::endl);

    auto pos_1 = simple_physics();
    auto pos_2 = simple_physics_2();
    hpw_log("pos 1: " << pos_1.x << " (" << std::hex << to_u32(pos_1.x)
      << "), " << pos_1.y << " (" << std::hex << to_u32(pos_1.y) << ")\n");
    hpw_log("pos 2: " << pos_2.x << " (" << std::hex << to_u32(pos_2.x)
      << "), " << pos_2.y << " (" << std::hex << to_u32(pos_2.y) << ")\n");

    hpw_assert(to_u32(a)        == 0x3f7adf50u);
    hpw_assert(to_u32(b)        == 0x4040018eu);
    hpw_assert(to_u32(mul_ab)   == 0x403c2902u);
    hpw_assert(to_u32(pow2_a)   == 0x3f75d8ebu);
    hpw_assert(to_u32(pow2_b)   == 0x41100255u);
    hpw_assert(to_u32(pow_ab)   == 0x3f70ec2cu);
    hpw_assert(to_u32(sqrt_a)   == 0x3f7d6c56u);
    hpw_assert(to_u32(sqrt_b)   == 0x3fddb4bdu);
    hpw_assert(to_u32(pi)       == 0x40490fdbu);
    hpw_assert(to_u32(sum_x3)   == 0x403c277cu);
    hpw_assert(to_u32(mul_x3)   == 0x3f70ec4au);
    hpw_assert(to_u32(sin_a)    == 0x3f549a61u);
    hpw_assert(to_u32(sin_b)    == 0x3e106923u);
    hpw_assert(to_u32(cos_a)    == 0x3f0e9aaau);
    hpw_assert(to_u32(cos_b)    == 0xbf7d7107u);
    hpw_assert(to_u32(atan2_ab) == 0x3ea1a615u);
    hpw_assert(to_u32(c)        == 0x3dfcd6e9u);
    hpw_assert(to_u32(d)        == 0x47543103u);
    hpw_assert(to_u32(sum_cd)   == 0x47543123u);
    hpw_assert(to_u32(sub_cd)   == 0xc75430e3u);
    hpw_assert(to_u32(mul_cd)   == 0x45d19261u);
    hpw_assert(to_u32(div_cd)   == 0x36188523u);
    hpw_assert(to_u32(sin_d)    == 0x3e87037fu);
    hpw_assert(to_u32(sqrt_d)   == 0x43691195u);

    // -O0
    hpw_assert(to_u32(pos_1.x)  == 0x3f7f97d6u);
    hpw_assert(to_u32(pos_1.y)  == 0xbd66d891u);
    /* на -O3 другие значения:
    hpw_assert(to_u32(pos_1.x)  == 0x3f7f97d5u);
    hpw_assert(to_u32(pos_1.y)  == 0xbd66d983u); */

    hpw_assert(to_u32(pos_2.x)  == 0xc459607bu);
    hpw_assert(to_u32(pos_2.y)  == 0x44a22933u);
  } catch (hpw::Error& e) {
    std::cerr << e.location << ": " << e.msg << std::endl;
  } catch (...) {
    std::cerr << "unregistered exception" << std::endl;
  }
}
