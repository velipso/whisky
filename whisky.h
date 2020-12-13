//
// Whisky - a collection of stateless RNGs
// by Sean Connelly (@velipso), https://sean.cm
// The Unlicense License
// Project Home: https://github.com/velipso/whisky
//

#ifndef WHISKY__H
#define WHISKY__H

#include <stdint.h>

// generates floats ranging [0, 1) using 23 bits of randomness
float whiskyf(uint32_t a);

// generates doubles ranging [0, 1) using 52 bits of randomness
double whiskyd(uint32_t a, uint32_t b);

//////////////////////////////////////////////////////////////////////////////
//
// RECOMMENDED HASHES
uint32_t whisky3x1(uint32_t x0);
//
//////////////////////////////////////////////////////////////////////////////

#endif
