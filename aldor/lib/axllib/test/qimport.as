-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
--> testphase scobind
--> testrun -l axllib
#pile

-- This file tests processing of qualified imports. It checks both
-- abnorm and the later handling in tinfer. We print the syntax after
-- scobind (and not after abnorm) because abnorm is currently run twice.

#include "axllib.as"

import from TextWriter
import
        string: Literal -> %
        <<    : (TextWriter, %) -> TextWriter
from String

import
        integer: Literal -> %
from Integer

import
        nil     : %
        cons    : (Integer, %) -> %
        <<      : (TextWriter, %) -> TextWriter
from List(Integer)

inline
        integer: Literal -> %
from Integer

inline
        nil     : %
        cons    : (Integer, %) -> %
from List(Integer)

export f : Integer -> Integer

f(n : Integer): Integer ==
    import from List Integer
    first rest cons(n, cons(n , nil))

print<<"hello"<<newline
print<<cons(3, cons(2, nil))<<newline<<newline

#if TestErrorsToo
print << rest cons(3,cons(2, nil))
#endif
