Final Reports
=============

The final hash functions have been tested with dieharder, for each axis, in both positive and
negative directions.  You can see the results in the
[reports/](https://github.com/velipso/whisky/tree/main/reports) directory.

Note that some individual tests are reported as weak.  This is okay!  If you look at SHA-256, it
even has a failed test!

It's important to understand that weak simply means the probability of the result (P-value) is less
than 1%.  Since hundreds of tests are performed, it is _expected_ to receive a few weak results.

In the case of the failed SHA-256 test, I would trust SHA-256 over dieharder :-P.

### Testing Multi-Dimensional Hashes

The dieharder test only accepts a stream of numbers.  How can we possibly test that a
multi-dimensional hash is high quality when there are billions of ways of navigating the space to
produce a 1D stream of numbers?

My answer is to test the functions under the worst possible scenarios, and if they pass, then
surely they'll be fine under normal use.

I start by only generating hash functions that use multiply, XOR, and bit shift.  Importantly, I
don't use addition, bit NOT, or XOR with a constant.

This means that setting an input to 0 is the _most pathological_ scenario.  Multiplying by 0 is
always 0, XORing with 0 is a no-op, and bit shifting 0 is a no-op.  As a result, every hash function
will return 0 if all inputs are 0!

This sounds bad!

But now we can easily test if a hash function is useful across a dimension.  Just set all the
_other_ inputs to 0, and use an increasing number along the axis of interest, and see if dieharder
likes the output.

I also test in the negative direction to ensure that inputs saturated with bits (i.e.,
`0xFFFFFFFF`) don't cause a failure.

When you read the [reports/](https://github.com/velipso/whisky/tree/main/reports), you'll notice the
functions pass along each axis, in both positive and negative directions.  This ensures that each
axis is contributing meaningfully to the output.
