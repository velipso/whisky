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

### Strategy

The strategy I used for finding the whisky functions was to try different qualities for 10
functions until I found a quality that had an average score around 300.  So for example, run this
command for `a`-`f`:

```
node whisky.js step 10 a 4 17; do:; done
```

Once finished, score it:

```
node whisky.js score 4 17 -s -n
```

If the average of the top 10 was over 300, then increase quality by one, and try again.  Once a good
quality was found, then run for much larger numbers, but reject failures and weak functions:

```
node whisky.js step 2000 a 4 18 -f -w 21; do:; done
```

The weak threshold (21) was chosen via: floor(272 × dimensions × 0.02) -- this aims for less
than 2% of the tests being marked weak.

### Summary of Results

| Dimension | Functions searched | Tests performed | Primary ID     | Alt ID         | Alt2 ID        |
|:---------:|-------------------:|----------------:|----------------|----------------|----------------|
|     1     |             60,000 |         142,325 | `a 1 8 7005`   | `b 1 8 5290`   | `f 1 8 5988`   |
|     2     |             19,000 |         315,331 | `b 2 12 379`   | `b 2 12 536`   | `e 2 12 2645`  |
|     3     |             87,000 |         322,805 | `f 3 15 3017`  | `a 3 15 13985` | `d 3 15 14685` |
|     4     |             60,000 |         212,305 | `e 4 18 3232`  | `f 4 18 144`   | `c 4 18 4092`  |
|     5     |            120,000 |         478,771 | `e 5 22 19801` | `b 5 22 16247` | `a 5 22 2845`  |
| **Total** |            323,000 |       1,471,537 | | | |
