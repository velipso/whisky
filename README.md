whisky
======

# Search in progress!

Whisky is a collection of stateless random number generators.  These can be thought of as tiny hash
functions, or pure noise functions.

Feel free to copy/paste them directly into your projects.

I generated hundreds of thousands of potential functions in each category, and ran them against the
[dieharder 3.31.1](http://webhome.phy.duke.edu/~rgb/General/dieharder.php) statistical tests.

This repo contains the single best function I could find in a given category.

### Note: I am still searching but this repo will contain the best functions as I find them!

Categories
==========

Whisky functions are in the format of:

```
uint32_t whiskyQUALITYxDIMENSION(uint32_t ...);

// examples:
uint32_t whisky3x1(uint32_t x0);
uint32_t whisky4x2(uint32_t x0, uint32_t x1);
```

The `QUALITY` value trades processing time for more unpredictable results (higher number means more
unpredictable using more operations).

The `DIMENSION` value is the number of input parameters.

Low quality functions are included in the repo!  If you want great general purpose functions, grab
the highest quality function available -- these functions pass _all_ the `dieharder` tests, and are
marked `RECOMMENDED` in the source code.

I.e., use `whisky3x1` instead of `whisky2x1`, unless you have a specific need for higher speed
functions.

Stateless RNG
=============

Why stateless RNGs?

For a cool talk on them, see
[Math for Game Programmers: Noise-Based RNG](https://www.youtube.com/watch?v=LWFzPP8ZbdU) by
Squirrel Eiserloh.

In summary, stateless RNGs are pure functions, which means they play nice with threads, and can be
accessed out of order.  They are not seeded like typical RNGs.

If you prefer having state, you can easily add it back in.  For example:

```
uint32_t nextValue(){
  static uint32_t n = 0;
  return whisky3x1(n++);
}
```

If you prefer having seeds, you can easily add that by increasing the dimension, and passing in the
seed as one of the dimensions.  For example:

```
static uint32_t seed = 0, n = 0;
uint32_t nextValue(){
  return whisky4x2(n++, seed);
}

void setSeed(uint32_t s){
  seed = s;
  n = 0;
}
```

Floating Point
==============

I've also provided two utility functions to create floating point values ranging from 0 (inclusive)
to 1 (exclusive).

```
float whiskyf(uint32_t a);
double whiskyd(uint32_t a, uint32_t b);
```

These can be used like:

```
float n1 = whiskyf(whisky3x1(123));
double n2 = whiskyd(whisky3x1(456), whisky3x1(789));
```
