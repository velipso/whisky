whisky
======

Whisky is a collection of stateless random number generators.  These can be thought of as tiny hash
functions, or pure noise functions.

Feel free to copy/paste them directly into your projects -- all you need is
[`whisky.h`](https://github.com/velipso/whisky/blob/main/whisky.h)!  Everything else in the repo is
supplemental.

I'm in the process of generating tens of thousands of functions, and analyzing them to find the best
ones using the [dieharder 3.31.1](http://webhome.phy.duke.edu/~rgb/General/dieharder.php)
statistical tests
([screenshot](https://raw.githubusercontent.com/velipso/whisky/main/screenshot.png)).

This repo will eventually contain the best functions I've found.

### NOTE: I am still actively searching!

Progress: 40%

| Dimension | Status |
|:---------:|:------:|
|     1     |   ⏳   |
|     2     |   ✅   |
|     3     |   ✅   |
|     4     |   ⏳   |
|     5     |   ⏳   |

API
===

The functions are named in the format of:

```c
uint32_t whiskyX[alt](...);
float whiskyXf(...);
double whiskyXd(...);
```

* `X` - the number of inputs (dimensions), from `1` to `5`
* `[alt]` - (optional) alternate hash function of equal quality

For example, `whisky2` is a hash function with two arguments, and the `whisky2alt` function is of
equal quality compared to `whisky2`.

Having an alternate is useful if you need a 64-bit result -- you can get 32-bits from `whisky2` and
another 32-bits from `whisky2alt` using the same input.

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
array in-place by setting input and output to the same array.

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
float randomValue = whisky1f(123);
double randomValue = whisky1d(123);
```

These will return the same value for the same input, but the `double` has more precision.

Final Reports
=============

The final hash functions have been tested with dieharder, for each axis, in both positive and
negative directions.  You can see the results in the `reports/` directory.

Note that some individual tests are reported as weak.  This is okay!  If you look at SHA-256, it
even has a failed test!

It's important to understand that weak simply means the probability of the result (P-value) is less
than 1%.  Since hundreds of tests are performed, it is _expected_ to receive a few weak results.

In the case of the failed SHA-256 test, I would trust SHA-256 over dieharder :-P.

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
uint32_t whisky1(uint32_t i0){
  TODO
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
2d or 3d array?  You can change the seed, but that isn't adding a dimension, that's just skipping
ahead in the 1d array.

The whisky collection provides multi-dimensional hashes, so you have `whisky2(x, y)`,
`whisky3(x, y, z)`, `whisky4(x, y, z, w)`, and `whisky5(x, y, z, w, v)`.

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

Search Script
=============

If you would like to reproduce my results, you can perform the same exact searches using the
`whisky.js` script and [node.js](https://nodejs.org).

Here's an example search:

```
node whisky.js step 100 a 2 12
```

This will (eventually) find the best function out of 100, for 2 dimensions, with 12 operators.  It
will perform a single step, and save the results to a file.  It will exit with an error if there
aren't any more steps to perform.

The `a` is simply a unique identifier so you can have multiple terminals open.  For example, I have
a six-core CPU, so I use six terminals, labeled `a`-`f`.

In order to keep searching, just put it in a loop in your shell:

```
while node whisky.js step 100 a 2 12; do :; done
```

You can kill it at any time, and it will resume where it left off.  If you want to start over, just
delete the state file associated with it (`state.a.2.12.json`).

To find the best function, use the `score` subcommand:

```
node whisky.js score 2 12
```

This will search across all the state files, and rank the functions.  Note that lower score is
better!

Once you know the best functions, you can print them with the `print` subcommand:

```
node whisky.js print b 2 12 379
```

The strategy I used for finding the whisky functions was to start small and only search across 10
functions.  Once that process finished, I increased the total to 50, 100, etc, until I was satisfied
with the results.

### Summary of Results

| Dimension | Functions searched | Tests performed | Primary ID    | Alternate ID   |
|:---------:|-------------------:|----------------:|---------------|----------------|
|     1     |                TBD |                 |               |                |
|     2     |             19,000 |         315,331 | `b 2 12 379`  | `b 2 12 536`   |
|     3     |             87,000 |         322,805 | `f 3 15 3017` | `a 3 15 13985` |
|     4     |                TBD |                 |               |                |
|     5     |                TBD |                 |               |                |

RNG Generator
=============

The code used to generate random hash functions is included in `whisky.js`, called `generateRNG()`.

The basic idea is to pick a random template for the next line of code, and keep adding lines until
the budget (quality) is used up:

```javascript
// possible templates for each line, where:
//   x0,x1  input variables
//   P      random large prime
//   S,T    random shift amount, where S+T = 32
const templates = [{
  cost: 2,
  line: '(x0 * P) ^ x1'
}, {
  cost: 3,
  lossy: true, // flag x1 as lossy, since bits are thrown away
  line: '(x0 * P) ^ (x1 >> S)'
}, {
  cost: 4,
  line: '(x0 * P) ^ (x1 >> S) ^ x1'
}, {
  cost: 5,
  line: '(x0 * P) ^ (x1 >> S) ^ (x1 << T)'
}];
```

If the quality is set to 12, it might achieve that by doing 5+2+3+2, for example.  The cost is just
a count of the number of operators in the line of code.

Notice that one template is marked as `lossy`.  This is because some of `x1`'s bits will be lost
due to the right shift.

It's important to track `lossy` so that `x1` is only replaced with a variable that has also been
used elsewhere.  That increases the likelihood that the hash will pass more tests, since all bits
are guaranteed to be used somewhere in the calculation.

Another important trick is here:

```javascript
// get a random element in a list, but favor the tail
const rndGetTail = a => {
  for (let i = a.length - 1; i >= 1; i--)
    if (rnd(0, 1)) return a[i];
  return a[0];
};
```

While constructing the hash function, all variables are added to a list, so that when `x0` and `x1`
are replaced, they can pull from any variable in scope.

However, notice that `rndGetTail` will heavily favor the tail of the list.  This is because the
tail will contain later variables, which have been mixed more.

Lastly, notice that the templates favor _right_ shift over _left_ shift.  You might think they're
equally useful, since they just move bits around, so who cares what direction they go in?

My testing indicates that's wrong though.  I suspect it's due to the nature of multiplication.  When
multiplying a value with lots of zeroes in the lower bits, those bits _stay_ zero.  The same isn't
true of zeros in the higher bits -- those get clobbered by the carry.  So right shift is better.

Fun Rant
========

If you'll indulge me, I'd like to rant a little bit about how much fun this project was :-).

What's the point?  You can make a good-enough hash function in a couple hours, so why bother with
all of this?

Simple: it's fun!

There are two main problems:

1. How to generate good RNG functions
2. How to navigate the search space wisely

Both of them are really challenging and interesting, and feed into each other.

I started by creating a completely haphazard RNG generator that just spit out random lines of code,
with random operators.  It was terrible!

Slowly, I made small optimizations, and tested along the way.

While it is frustrating to have to run tests for a day before making a decision on whether an idea
is any good, that's also what makes it entertaining.  It's so costly to try an idea, it forces you
to tread carefully.

Creating the algorithm for testing was also fun too.  At first I would just generate a random
function and run all tests against it.  Eventually I settled on a breadth-first search that ran the
fastest tests firsts, so I could reject bad functions as quickly as possible.

I understand that this was all unnecessary in the grand scheme of things, but who cares?  I did it
in my free time and had a blast :-).
