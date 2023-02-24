#include "foamlib"
#pile

Rx: Category == with
AM: Category == with
A1: Category == with

LinExp(R: Rx): Category == with
    if R has A1 then AM
    if R has AM then A1

Foo(A: AM): () == never

Int: Join(Rx, LinExp Int) with == add
Foo Int

-- Checking 'Foo Int', asks'Int has AM'?
--> Parents(Int) has AM
--> 'AM (if Int has A1)' has AM
--> 'A1 (if Int has AM)' has A1
--> 'Int has AM'

