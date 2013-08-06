-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib.as"

SI ==> SingleInteger;
import from SI;
import from List(SI);

print << [for i in 1..3 repeat i] << newline
