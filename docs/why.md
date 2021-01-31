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
static uint32_t whisky1(uint32_t i0){
  uint32_t z0 = (i0 * 1831267127) ^ i0;
  uint32_t z1 = (z0 * 3915839201) ^ (z0 >> 20);
  uint32_t z2 = (z1 * 1561867961) ^ (z1 >> 24);
  return z2;
}
```

There are no global variables -- instead, `i0` is passed in and hashed.

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
2D or 3D array?  You can change the seed, but that isn't adding a dimension, that's just skipping
ahead in the 1D array.

The whisky collection provides multi-dimensional hashes, so you have `whisky2(x, y)`,
`whisky3(x, y, z)`, `whisky4(x, y, z, w)`, and `whisky5(x, y, z, w, v)`.

This also makes it easy to seed -- for example, if you want a seeded 2D array, use `whisky3` and
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
