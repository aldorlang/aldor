-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testcomp
--> testrun -laxllib

#include "axllib"

-- AbDom has type with { Rep:SingleInteger, foo: ... } because
-- we have placed no restriction on its type (on the LHS).
AbDom == add
{
   Rep == SingleInteger;
   import from Rep;

   foo(x:%):% == per ((rep x) + 1);
   coerce(x:%):SingleInteger == rep x;
   coerce(x:SingleInteger):% == per x;
}


import from AbDom, SingleInteger;
print << ((foo(foo(42::AbDom)))::SingleInteger) << newline;
