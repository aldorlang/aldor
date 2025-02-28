# Demo type library

Experimental type library.
- Supplies an expression type and a number of type inference approaches

This exists for several reasons
- experimenting with a type inference implementation
- avoids having to commit C code
- allows a second implementation & easier testing
- test for compiler - the more code it sees, the better

Goals:

Something like
(for x in l)
    >> filter(x +-> even? x)
    >> map(x +-> x + 1)
    >> collect(setCollector())

## Current iteration: Lambda inference

ToDo:
A: Top down final type inference
B: error generation
C: cleaner implementation of TPoss/ParamSet
D: domains, categories, conditional exports
E: Test cases


A: Type inference
- apply needs to be more complete
- Test cases:
-- Lambda.. Fail on conflicting use (L (x) (pair (+ x 1) (first x)))
-- Lambda.. Fail where ambiguous (L (x) (+ x x))
-- Lambda.. Success on merge (L (x) (pair (first x) x))

B: Error propagation
- .. Some kind of 'cause' or causality chain for errors

C: Test cases
.. See above; (for x in l) >> filter(...) >> map(...) >> collect(...)

foo >> filter(isEven)  ==> filter(isEven, foo)

collect: F(A) -> Sink A
map: (F(A), T(A)) -> Sink A
filter: (F(A), T(A)) -> Sink A

a < b < c

<: (A, A) -> (A, Bool)
<: ((A, Bool), A) -> Bool

--

bar(): () ==
    aux(x: Integer) == if x = 0 then 1 else aux(x-1)
    f(blah)