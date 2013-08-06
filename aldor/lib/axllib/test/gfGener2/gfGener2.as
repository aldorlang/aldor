-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"

Foo : with
{
   foo: SingleInteger -> List(Record(r:SingleInteger));
}
== add
{
   foo(u:SingleInteger):List(Record(r:SingleInteger)) ==
      [record(x) for x in 1..u];
}

import from Foo;
import from List(Record(r:SingleInteger));

for rec in foo(10) repeat
   print << "   [" << rec.r << "]" << newline;
