//
// Copyright 2020 Sean Connelly (@velipso), https://sean.cm
// MIT License
// Project Home: https://github.com/velipso/whisky
//

#include "whisky.h"

uint32_t whisky3x1(uint32_t x0){
	uint32_t z0 = (x0 >> 7) ^ (x0 * 243968551);
	uint32_t z1 = (z0 >> 12) ^ (z0 * 297120911);
	return (z1 >> 17) ^ (z1 * 1609291951);
}
