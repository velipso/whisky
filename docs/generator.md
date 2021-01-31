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
