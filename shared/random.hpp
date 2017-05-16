#pragma once

#include <cstdint>

// Simple random generator which is much better than build in rand() from C-library.
// Still, this generator is NOT thread safe.
// Implementation from: https://de.wikipedia.org/wiki/Xorshift
inline uint64_t xorshift64()
{
	static uint64_t x64 = 88172645463325252ull;
	x64 ^= x64 << 13;
	x64 ^= x64 >> 7;
	x64 ^= x64 << 17;
	return x64;
}

// Create a uniform number in [0,1] including 0 and 1
inline float urand()
{
	return float((xorshift64() & 0x007fffff) / double(0x007fffff));
}