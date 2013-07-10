-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"

import from List(Record(r:SingleInteger));

foo(u:SingleInteger):List(Record(r:SingleInteger)) ==
      [[x] for x in 1..u];

for rec in foo(10) repeat
   print << "   [" << rec.r << "]" << newline;
