whisky
======

Whisky is a collection of stateless random number generators.  These can be thought of as tiny hash
functions, or pure noise functions.

Feel free to copy/paste them directly into your projects.

I'm in the process of generating tens of thousands of functions, and analyzing them to find the best
ones using the [dieharder 3.31.1](http://webhome.phy.duke.edu/~rgb/General/dieharder.php)
statistical tests.

This repo will eventually contain the best functions I've found.

### NOTE: I am still actively searching!

The functions provided are my best so far, but are subject to change!  Come back in 6 months and it
will probably be finalized :-).

API
===

The functions are named in the format of:

```c
uint32_t whisky<D>[alt][_fast](...);
float whisky<D>f[_fast](...);
double whisky<D>d[_fast](...);
```

* `<D>` - the number of inputs (dimensions)
* `[alt]` - (optional) alternate hash function of equal quality
* `[_fast]` - (optional) faster hash function of lower quality

For example, `whisky1` is a recommended hash function with one argument.  The `whisky1alt` function
is also recommended and of equal quality compared to `whisky1`.

The `whisky1_fast` function also takes one argument, but is lower quality.  And `whisky1alt_fast` is
an alternate that is also lower quality.

All the functions are in the single header file `whisky.h` and declared `static`.  This allows the
compiler to inline functions where possible.

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
float randomValue = whisky1f(123);
double randomValue = whisky1d(123);
```

These will return the same value for the same input, but the `double` has more precision.

Stateless RNG
=============

Most RNGs have state.  For example, the
[smush](https://gist.github.com/velipso/d112165a26b45244a65298933c0349a4) function passes a lot of
statistical tests:

```c
static uint32_t seed = 0, i = 0;
uint32_t smush(){
  uint32_t m = 0x5bd1e995;
  uint32_t k = i++ * m;
  seed = (k ^ (k >> 24) ^ (seed * m)) * m;
  return seed ^ (seed >> 13);
}
```

Notice that `seed` and `i` are global variables, and no arguments are passed into `smush`.

Compare that to `whisky1`, provided in this repo:

```c
uint32_t whisky1(uint32_t x0){ // e 441
  uint32_t z0 = (x0 >> 4) ^ (x0 * 1067874637);
  uint32_t z1 = (z0 >> 6) ^ (z0 * 1855142719);
  return (z1 >> 19) ^ (z1 * 216744991);
}
```

There are no global variables -- instead, `x0` is passed in and hashed.

Why does it matter?

Well, at the very least, the whisky hash functions can always _become_ stateful.  For example:

```c
static uint32_t seed = 0;
uint32_t whisky1_stateful(){
  return whisky1(seed++);
}
```

They have been designed and tested specifically with incremental inputs, so this is exactly what
they're good at.

However, there are additional benefits that traditional RNGs _can't_ do:

### 1. Numbers can be accessed in any order

If you want the 1000th number from `smush`, you have to loop around 1000 times.  God help you if you
want to rewind.

If you want the 1000th number from `whisky1`, you just do `whisky1(1000)`.

You can think of `whisky1` as a 1-dimensional array of random numbers with 2<sup>32</sup> elements.
It's easy to jump around and access any element without extra cost.

### 2. Multi-dimensional

Likewise, `smush` can be thought of as a 1-dimensional array -- but that's it.  What if you want a
2d or 3d array?  You can change the seed, but that isn't adding a dimension, that's just skipping
ahead in the 1d array.

The whisky collection will provide multi-dimensional hashes, so you will have `whisky2(x, y)`,
`whisky3(x, y, z)`, and `whisky4(x, y, z, w)`.

This also makes it easy to seed -- for example, if you want a seeded 2d array, use `whisky3` and
pass the seed as a parameter `whisky3(x, y, seed)`.

### 3. Pure functions

Since there isn't any global state, the whisky functions are easy for compilers to optimize/inline,
and are thread-safe.  There are no side-effects!

### 4. Independent procedural generation

Suppose you create a function that draws random flowers.

If you use the same seed, you will get the same flower -- that's true of both `smush` and `whisky1`.

What happens when you tweak it later?

If you insert a new line of code that uses `smush`, you have _changed_ the sequence of random
numbers the rest of the generation process uses.

Suppose something else happens after flower generation that is more important -- like the location
of a town on a map.  Tweaking the flower generation will relocate villages!  That's not good.

With whisky, you can break these dependencies because the random numbers are only based on the
parameters passed in.  This is a crucial property for games with procedural content.

Search Space
============

* `whisky1` / `whisky1alt` - searched 3000 functions
* `whisky1_fast` / `whisky1alt_fast` - searched 6000 functions
