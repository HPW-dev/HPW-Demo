#include "xorshift.hpp"

std::uint32_t xorshift128(xorshift128_state& state) {
	// Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs"
	std::uint32_t t = state.d;

	std::uint32_t const s = state.a;
	state.d = state.c;
	state.c = state.b;
	state.b = s;

	t ^= t << 11;
	t ^= t >> 8;
	return state.a = t ^ s ^ (s >> 19);
}
