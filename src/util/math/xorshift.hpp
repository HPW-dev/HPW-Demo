#pragma once
#include <cstdint>

struct xorshift128_state {
  std::uint32_t a {124'521};
  std::uint32_t b {623'345};
  std::uint32_t c {641'021};
  std::uint32_t d {124'246};
};

// The state array must be initialized to not be all zero
std::uint32_t xorshift128(xorshift128_state& state);
