-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib.as"
#pile

import from Character
import from SingleInteger

-- Test character printing.
print<<1<<space<<1<<newline

-- Test literals.
s: String := "Hello"
print<<s<<newline

-- Test selection.
print<<1<<s.1<<1<<s.2<<1<<s.3<<1<<s.4<<1<<s.5<<1<<newline
print<<s(2..4)<<newline

-- Test "new".
spaces: String := new(10, space)
print<<1<<spaces<<1<<newline

-- Test "concat"
s := concat("Hello", " ", "there", "!")
print<<s<<newline
