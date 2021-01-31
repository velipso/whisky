whisky
======

Whisky is a collection of stateless random number generators.  These can be thought of as tiny hash
functions, or pure noise functions.  Stateless RNGs are useful for a
[variety of reasons](https://github.com/velipso/whisky/blob/master/docs/why.md).

All you need is [`whisky.h`](https://github.com/velipso/whisky/blob/main/whisky.h)!  It includes
the hash functions, a short SHA-256 implementation, and some utility functions for creating floating
point numbers.

Everything else in this repo is supplemental for those curious about how these functions were
generated and tested.

These functions were found by searching through
[hundreds of thousands](https://github.com/velipso/whisky/blob/master/docs/search.md) of
candidates.  They have been tested against the
[dieharder 3.31.1](http://webhome.phy.duke.edu/~rgb/General/dieharder.php)
statistical tests
([screenshot](https://raw.githubusercontent.com/velipso/whisky/main/screenshot.png)), and
[pass](https://github.com/velipso/whisky/blob/master/docs/reports.md) with flying colors.

API
===

The functions are named in the format of:

```c
static uint32_t whiskyX(...);
static uint32_t whiskyXalt(...);
static uint32_t whiskyXalt2(...);
static float whiskyXf(...);
static double whiskyXd(...);
```

* `X` - the number of inputs (dimensions), from 1 to 5
* `alt` / `alt2` - alternate hash functions of equal quality

For example, `whisky2` is a hash function with two inputs:

```c
static uint32_t whisky2(uint32_t i0, uint32_t i1){
  // ...mix up the inputs i0 and i1...
  return mixedValue;
}
```

The `whisky2alt` hash function also takes two inputs, and is equal quality compared to `whisky2`
-- it just mixes things differently.  The same is true for `whisky2alt2`.

Having an alternate is useful if you need more random bits from the same input.  You can get 32-bits
from `whisky2`, another 32-bits from `whisky2alt`, and another 32-bits from `whisky2alt2` using the
same input.

All the functions are in the single header file `whisky.h` and declared `static`.  This allows the
compiler to inline functions where possible.

If you need higher dimensions, you can chain hashes.  For example, if you need 9-dimensions, you
could do something like `whisky2(whisky4(...), whisky5(...))`.

SHA-256
=======

The plain `whiskyX` hashes are not cryptographically secure, so just for fun, I've also provided a
single chunk implementation of [SHA-256](https://en.wikipedia.org/wiki/SHA-2) -- which is complete
overkill for games, but it's small (about 50 lines) and easy to include.

```c
void whisky_sha256(const uint32_t input[8], uint32_t output[8]);
```

This is not a full implementation -- it cannot take an arbitrary length input.  It is specifically
limited to perform a hash on 32 bytes of input, and outputs the 32 byte hash.  You can hash an
array in-place by setting `input` and `output` to the same array.

It's obviously much slower than the `whiskyX` functions, but it's still quite fast!  And of course
the quality is ridiculously good.

Floating Point
==============

I've also provided two utility functions to create floating point values ranging from 0 (inclusive)
to 1 (exclusive).

```c
float whiskyf(uint32_t a);
double whiskyd(uint32_t a, uint32_t b);
```

Simply pass in 32 or 64 random bits (via `a`/`b`), and get out a floating point number.

For convenience, I've wired up the hashing functions to the floating point functions.  For example,
you can call:

```c
float  randomFloat  = whisky2f(123, 456); // 0.80855942
double randomDouble = whisky2d(123, 456); // 0.8085594523896744
```

These will return the same value for the same input, but the `double` has more precision.

More Info
=========

* [Why a stateless RNG?](https://github.com/velipso/whisky/blob/master/docs/why.md)
* [Final Reports](https://github.com/velipso/whisky/blob/master/docs/reports.md)
* [Search Script](https://github.com/velipso/whisky/blob/master/docs/search.md)
* [RNG Generator](https://github.com/velipso/whisky/blob/master/docs/generator.md)
* [Why do this at all?](https://github.com/velipso/whisky/blob/master/docs/fun.md)
