-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  SrcPos     -- #line info for C code generation
--> testgen c -Clines -Zdb -I ../lib/libaxllib
#pile

#include "../../axllib/include/axllib.as"

f(n: Integer): Integer == (n + 1) * n

import from Integer
f 1
