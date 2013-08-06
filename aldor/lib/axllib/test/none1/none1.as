-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

-- This file tests that a function which returns no values
-- actually returns no values.

#include "axllib"
import from SingleInteger;

f () : () == return 0
