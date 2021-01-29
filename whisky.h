//
// Whisky - a collection of stateless RNGs
// by Sean Connelly (@velipso), https://sean.cm
// The Unlicense License
// Project Home: https://github.com/velipso/whisky
//

#ifndef WHISKY__H
#define WHISKY__H

#include <stdint.h>

//
// HASH FUNCTIONS
//
// Note: both the primary and alternate(s) pass all dieharder tests and are of equal quality
//

static uint32_t whisky1(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky1alt(uint32_t i0){
	return i0; // TODO: this!
}

static uint32_t whisky2(uint32_t i0, uint32_t i1){
	uint32_t z0 = (i1 * 1833778363) ^ i0;
	uint32_t z1 = (z0 *  337170863) ^ (z0 >> 13) ^ z0;
	uint32_t z2 = (z1 *  620363059) ^ (z1 >> 10);
	uint32_t z3 = (z2 *  232140641) ^ (z2 >> 21);
	return z3;
}

static uint32_t whisky2alt(uint32_t i0, uint32_t i1){
	uint32_t z0 = (i0 *  477119449) ^ i1;
	uint32_t z1 = (z0 *  923076359) ^ z0;
	uint32_t z2 = (z1 * 2089390813) ^ (z0 >> 13);
	uint32_t z3 = (z2 *  811285039) ^ i1;
	uint32_t z4 = (z3 *  210814523) ^ (z3 >> 19);
	return z4;
}

static uint32_t whisky3(uint32_t i0, uint32_t i1, uint32_t i2){
	uint32_t z0 = (i0 *  271754957) ^ i2;
	uint32_t z1 = (i1 *  772196531) ^ z0;
	uint32_t z2 = (z1 *  261525307) ^ (i2 >> 28);
	uint32_t z3 = (z2 *  402089977) ^ (z2 >> 12);
	uint32_t z4 = (z3 *  239548357) ^ z2;
	uint32_t z5 = (z4 * 3133165447) ^ (z4 >> 24);
	return z5;
}

static uint32_t whisky3alt(uint32_t i0, uint32_t i1, uint32_t i2){
	uint32_t z0 = (i2 *  535876997) ^ i0;
	uint32_t z1 = (i1 *  162137881) ^ z0;
	uint32_t z2 = (z1 *  273200869) ^ (z1 >> 11);
	uint32_t z3 = (z2 * 1031467691) ^ (z2 >> 24) ^ (z2 <<  8);
	uint32_t z4 = (z3 * 1663048183) ^ (z3 >> 17);
	return z4;
}

static uint32_t whisky4(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	uint32_t z0 = (i1 * 1698371749) ^ (i3 >> 13) ^ i3;
	uint32_t z1 = (i0 *  825359837) ^ z0;
	uint32_t z2 = (i2 *  185995471) ^ z1;
	uint32_t z3 = (z2 * 2305326427) ^ z2;
	uint32_t z4 = (z3 *  310104779) ^ (z2 >> 13);
	uint32_t z5 = (z4 *  188519207) ^ z1;
	uint32_t z6 = (z5 *  959198237) ^ (z5 >> 19);
	return z6;
}

static uint32_t whisky4alt(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	uint32_t z0 = (i1 * 1204240987) ^ i3;
	uint32_t z1 = (i2 * 4010549321) ^ i0;
	uint32_t z2 = (z1 *  143320703) ^ z0;
	uint32_t z3 = (z2 *  959583701) ^ (z2 >> 13) ^ z2;
	uint32_t z4 = (z3 *  199604161) ^ (z3 >> 24);
	uint32_t z5 = (z4 *  192033599) ^ z0;
	uint32_t z6 = (z5 * 2308741217) ^ (z4 >> 20);
	return z6;
}

static uint32_t whisky5(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	uint32_t z0 = (i1 *  367353443) ^ i3;
	uint32_t z1 = (z0 * 2733269467) ^ i4;
	uint32_t z2 = (i2 *  212907043) ^ z1;
	uint32_t z3 = (i0 *  134452789) ^ z2;
	uint32_t z4 = (z3 *  859833071) ^ z3;
	uint32_t z5 = (z4 * 2868415003) ^ z2;
	uint32_t z6 = (z5 *  232681019) ^ (z3 >> 12);
	uint32_t z7 = (z6 *  219317647) ^ z5;
	uint32_t z8 = (z7 * 1558475939) ^ z6;
	uint32_t z9 = (z8 * 3631493917) ^ (z8 >> 19);
	return z9;
}

static uint32_t whisky5alt(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	uint32_t z0 = (i4 *  907294499) ^ i2;
	uint32_t z1 = (i3 * 2422727737) ^ i1;
	uint32_t z2 = (z1 * 1912919969) ^ z0;
	uint32_t z3 = (i0 * 2982886499) ^ z2;
	uint32_t z4 = (z3 * 1058057851) ^ z3;
	uint32_t z5 = (z4 * 1205991539) ^ z3;
	uint32_t z6 = (z5 * 1068665347) ^ (z5 >> 16) ^ (z5 << 16);
	uint32_t z7 = (z6 * 4277956667) ^ z2;
	uint32_t z8 = (z7 *  952786151) ^ (z7 >>  7);
	return z8;
}

static uint32_t whisky5alt2(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	uint32_t z0 = (i3 * 2364625957) ^ i0;
	uint32_t z1 = (i2 * 1831517539) ^ i4;
	uint32_t z2 = (z0 *  526395601) ^ z1;
	uint32_t z3 = (i1 *  265671227) ^ (z2 >> 23) ^ z2;
	uint32_t z4 = (z3 *  228188333) ^ (z3 >> 27) ^ z3;
	uint32_t z5 = (z4 * 4046791003) ^ z1;
	uint32_t z6 = (z5 *  466887391) ^ (z5 >> 14);
	uint32_t z7 = (z6 *  430028471) ^ (z6 >> 17);
	return z7;
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

//
// UTILITY FUNCTIONS
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
static float whisky2f(uint32_t i0, uint32_t i1){
	return whiskyf(whisky2(i0, i1));
}
static double whisky2d(uint32_t i0, uint32_t i1){
	return whiskyd(whisky2(i0, i1), whisky2alt(i0, i1));
}
static float whisky3f(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyf(whisky3(i0, i1, i2));
}
static double whisky3d(uint32_t i0, uint32_t i1, uint32_t i2){
	return whiskyd(whisky3(i0, i1, i2), whisky3alt(i0, i1, i2));
}
static float whisky4f(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyf(whisky4(i0, i1, i2, i3));
}
static double whisky4d(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3){
	return whiskyd(whisky4(i0, i1, i2, i3), whisky4alt(i0, i1, i2, i3));
}
static float whisky5f(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyf(whisky5(i0, i1, i2, i3, i4));
}
static double whisky5d(uint32_t i0, uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4){
	return whiskyd(whisky5(i0, i1, i2, i3, i4), whisky5alt(i0, i1, i2, i3, i4));
}

#endif
