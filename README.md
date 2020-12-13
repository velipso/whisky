whisky
======

### Note: Work in progress! Functions might change!

Whisky is a collection of stateless random number generators.  These can be thought of as tiny hash
functions, or pure noise functions.

Feel free to copy/paste them directly into your projects.

I'm in the process of generating tens of thousands of functions, and analyzing them to find the best
ones using the [dieharder 3.31.1](http://webhome.phy.duke.edu/~rgb/General/dieharder.php)
statistical tests.

This repo will eventually contain the best functions I've found.

Floating Point
==============

I've also provided two utility functions to create floating point values ranging from 0 (inclusive)
to 1 (exclusive).

```c
float whiskyf(uint32_t a);
double whiskyd(uint32_t a, uint32_t b);
```

These can be used like:

```c
float n1 = whiskyf(whisky3x1(123));
double n2 = whiskyd(whisky3x1(456), whisky3x1(789));
```
