
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).1999.
--> testerrs

#include "axllib.as"

f():() == {}

-- This used to segfault the compiler. It ought to fail
-- with an error about no meaning for i.
i := f();

