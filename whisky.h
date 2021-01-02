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

static uint32_t whisky1(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky1alt(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky2(uint32_t i0, uint32_t i1){
	return i0 + i1; // TODO: this!
}

static uint32_t whisky2alt(uint32_t i0, uint32_t i1){
	return i0 + i1; // TODO: this!
}

static uint32_t whisky3(uint32_t i0, uint32_t i1, uint32_t i2){
	return i0 + i1 + i2; // TODO: this!
}

static uint32_t whisky3alt(uint32_t i0, uint32_t i1, uint32_t i2){
	return i0 + i1 + i2; // TODO: this!
}

static uint32_t whisky4(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return i0 + i1 + i2 + i3; // TODO: this!
}

static uint32_t whisky4alt(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return i0 + i1 + i2 + i3; // TODO: this!
}

static uint32_t whisky5(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return i0 + i1 + i2 + i3 + i4; // TODO: this!
}

static uint32_t whisky5alt(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return i0 + i1 + i2 + i3 + i4; // TODO: this!
}

// performs a SHA-256 hash on exactly 32 bytes
// input and output can be the same array to hash in-place
static void whisky_sha256(const uint32_t input[8], uint32_t output[8]){
	const uint32_t k[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
		0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
		0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
		0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
		0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
		0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
		0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
		0xc67178f2
	};

	uint32_t a = 0x6a09e667, b = 0xbb67ae85, c = 0x3c6ef372, d = 0xa54ff53a;
	uint32_t e = 0x510e527f, f = 0x9b05688c, g = 0x1f83d9ab, h = 0x5be0cd19;
	uint32_t x, y, m[64] = {
		input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7],
		0x80000000, 0, 0, 0, 0, 0, 0, 0x100
	};

	for (int i = 16; i < 64; i++){
		m[i] = m[i - 16] + m[i - 7] +
			(((m[i - 2] >> 17) | (m[i - 2] << 15)) ^
			((m[i - 2] >> 19) | (m[i - 2] << 13)) ^
			(m[i - 2] >> 10)) +
			(((m[i - 15] >> 7) | (m[i - 15] << 25)) ^
			((m[i - 15] >> 18) | (m[i - 15] << 14)) ^
			(m[i - 15] >> 3));
	}

	for (int i = 0; i < 64; i++){
		x = h + k[i] + m[i] +
			((e & f) ^ (~e & g)) +
			(((e >> 6) | (e << 26)) ^
			((e >> 11) | (e << 21)) ^
			((e >> 25) | (e << 7)));
		y = ((a & b) ^ (a & c) ^ (b & c)) +
			(((a >> 2) | (a << 30)) ^
			((a >> 13) | (a << 19)) ^
			((a >> 22) | (a << 10)));
		h = g; g = f; f = e; e = d + x;
		d = c; c = b; b = a; a = x + y;
	}

	output[0] = 0x6a09e667 + a; output[1] = 0xbb67ae85 + b;
	output[2] = 0x3c6ef372 + c; output[3] = 0xa54ff53a + d;
	output[4] = 0x510e527f + e; output[5] = 0x9b05688c + f;
	output[6] = 0x1f83d9ab + g; output[7] = 0x5be0cd19 + h;
}

//////////////////////////////////////////////////////////////////////////////
//
// FASTER HASHES OF LOWER QUALITY
//
// These fail some statistical tests, which you might not care about, but are
// best in their category (as far as I could find)
//

static uint32_t whisky1_fast(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky1alt_fast(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky2_fast(uint32_t i0, uint32_t i1){
	return i0 + i1; // TODO: this!
}

static uint32_t whisky2alt_fast(uint32_t i0, uint32_t i1){
	return i0 + i1; // TODO: this!
}

static uint32_t whisky3_fast(uint32_t i0, uint32_t i1, uint32_t i2){
	return i0 + i1 + i2; // TODO: this!
}

static uint32_t whisky3alt_fast(uint32_t i0, uint32_t i1, uint32_t i2){
	return i0 + i1 + i2; // TODO: this!
}

static uint32_t whisky4_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return i0 + i1 + i2 + i3; // TODO: this!
}

static uint32_t whisky4alt_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return i0 + i1 + i2 + i3; // TODO: this!
}

static uint32_t whisky5_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return i0 + i1 + i2 + i3 + i4; // TODO: this!
}

static uint32_t whisky5alt_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return i0 + i1 + i2 + i3 + i4; // TODO: this!
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
static float whisky1f(uint32_t i0){
	return whiskyf(whisky1(i0));
}
static double whisky1d(uint32_t i0){
	return whiskyd(whisky1(i0), whisky1alt(i0));
}
static float whisky1f_fast(uint32_t i0){
	return whiskyf(whisky1_fast(i0));
}
static double whisky1d_fast(uint32_t i0){
	return whiskyd(whisky1_fast(i0), whisky1alt_fast(i0));
}
static float whisky2f(uint32_t i0, uint32_t i1){
	return whiskyf(whisky2(i0, i1));
}
static double whisky2d(uint32_t i0, uint32_t i1){
	return whiskyd(whisky2(i0, i1), whisky2alt(i0, i1));
}
static float whisky2f_fast(uint32_t i0, uint32_t i1){
	return whiskyf(whisky2_fast(i0, i1));
}
static double whisky2d_fast(uint32_t i0, uint32_t i1){
	return whiskyd(whisky2_fast(i0, i1), whisky2alt_fast(i0, i1));
}
static float whisky3f(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyf(whisky3(i0, i1, i2));
}
static double whisky3d(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyd(whisky3(i0, i1, i2), whisky3alt(i0, i1, i2));
}
static float whisky3f_fast(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyf(whisky3_fast(i0, i1, i2));
}
static double whisky3d_fast(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyd(whisky3_fast(i0, i1, i2), whisky3alt_fast(i0, i1, i2));
}
static float whisky4f(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyf(whisky4(i0, i1, i2, i3));
}
static double whisky4d(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyd(whisky4(i0, i1, i2, i3), whisky4alt(i0, i1, i2, i3));
}
static float whisky4f_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyf(whisky4_fast(i0, i1, i2, i3));
}
static double whisky4d_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyd(whisky4_fast(i0, i1, i2, i3), whisky4alt_fast(i0, i1, i2, i3));
}
static float whisky5f(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyf(whisky5(i0, i1, i2, i3, i4));
}
static double whisky5d(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyd(whisky5(i0, i1, i2, i3, i4), whisky5alt(i0, i1, i2, i3, i4));
}
static float whisky5f_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyf(whisky5_fast(i0, i1, i2, i3, i4));
}
static double whisky5d_fast(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyd(whisky5_fast(i0, i1, i2, i3, i4), whisky5alt_fast(i0, i1, i2, i3, i4));
}

#endif
