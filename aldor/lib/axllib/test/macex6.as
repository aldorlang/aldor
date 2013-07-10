-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex
--> testerrs
#pile

-- This file test detection of circular macro expansions.

-- By themselves, the definitions are allowed to exist.

macro y == y

macro
        q == e(s)
        r == g(q)
        s == h(r)

#if TestErrorsToo
-- But the errors are detected on expansion.

y+1
q + r - s
#endif

-- Non-circular macros (parameter x to f is correctly handled)

macro f(x) == x + 1

g(x: I): I == f(x)

macro per x == x @ T

zz(x: Integer): Integer == per G x

macro razob(a,b) == per(per(a) + per(b))

razob(zz(1), zz(2))
