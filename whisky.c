//
// Copyright 2020 Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/whisky
//

#include "whisky.h"

float whiskyf(uint32_t a){
	union { uint32_t i; float f; } u = { .i = 0x3F800000 | (a >> 9) };
	return u.f - 1.0f;
}

double whiskyd(uint32_t a, uint32_t b){
	uint64_t m = a;
	m = (m << 20) | (b >> 12);
	union { uint64_t i; double d; } u = { .i = (UINT64_C(0x3FF) << 52) | m };
	return u.d - 1.0;
}

//////////////////////////////////////////////////////////////////////////////
//
// RECOMMENDED 1D HASH
uint32_t whisky3x1(uint32_t x0){
	uint32_t z0 = (x0 >> 7) ^ (x0 * 243968551);
	uint32_t z1 = (z0 >> 12) ^ (z0 * 297120911);
	return (z1 >> 17) ^ (z1 * 1609291951);
}
//
//////////////////////////////////////////////////////////////////////////////
