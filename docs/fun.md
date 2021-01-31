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

I understand that this was all unnecessary in the grand scheme of things, but who cares?  It was a
welcome distraction from the pandemic, and filled some time.
