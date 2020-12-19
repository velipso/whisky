//
// Whisky - a collection of stateless RNGs
// by Sean Connelly (@velipso), https://sean.cm
// The Unlicense License
// Project Home: https://github.com/velipso/whisky
//

#ifndef WHISKY__H
#define WHISKY__H

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
//
// RECOMMENDED HASHES
//
// These pass the dieharder statistical tests
//
// Alternate functions (whiskyXalt) are just as good as their counterparts,
// just different... enjoy the variety!
//
static uint32_t whisky1(uint32_t x0){ // c 461
  uint32_t z0 = (x0 >> 6) ^ (x0 * 180263917);
  uint32_t z1 = (z0 >> 8) ^ (z0 * 443342311);
  return (z1 >> 24) ^ (z1 * 254956981);
}

static uint32_t whisky1alt(uint32_t x0){ // e 441
  uint32_t z0 = (x0 >> 4) ^ (x0 * 1067874637);
  uint32_t z1 = (z0 >> 6) ^ (z0 * 1855142719);
  return (z1 >> 19) ^ (z1 * 216744991);
}

//////////////////////////////////////////////////////////////////////////////
//
// FASTER HASHES OF LOWER QUALITY
//
// These fail some statistical tests, which you might not care about, but are
// best in their category (as far as I could find)
//
static uint32_t whisky1_fast(uint32_t x0){ // c 671
  uint32_t z0 = (x0 << 6) ^ (x0 * 567962861);
  return (z0 >> 11) ^ (z0 * 1798600487);
}

static uint32_t whisky1alt_fast(uint32_t x0){ // e 855
  uint32_t z0 = (x0 >> 5) ^ (x0 * 212456281);
  return (z0 >> 10) ^ (z0 * 1161455063);
}

//////////////////////////////////////////////////////////////////////////////
//
// UTILITY FUNCTIONS
//
// Functions for constructing floating point numbers
//

// generates floats ranging [0, 1) using 23 bits of randomness passed in
static float whiskyf(uint32_t a){
	union { uint32_t i; float f; } u = { .i = 0x3F800000 | (a >> 9) };
	return u.f - 1.0f;
}

// generates doubles ranging [0, 1) using 52 bits of randomness passed in
static double whiskyd(uint32_t a, uint32_t b){
	uint64_t m = a;
	m = (m << 20) | (b >> 12);
	union { uint64_t i; double d; } u = { .i = (UINT64_C(0x3FF) << 52) | m };
	return u.d - 1.0;
}

// convenience wrappers
static float whisky1f(uint32_t x0){
	return whiskyf(whisky1(x0));
}
static double whisky1d(uint32_t x0){
	return whiskyd(whisky1(x0), whisky1alt(x0));
}
static float whisky1f_fast(uint32_t x0){
	return whiskyf(whisky1_fast(x0));
}
static double whisky1d_fast(uint32_t x0){
	return whiskyd(whisky1_fast(x0), whisky1alt_fast(x0));
}

#endif
