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


-- The idea is to define MyDom which provides a restricted view of
-- AbDom. Unfortunately we need the "add" to persuade the compiler
-- to recognise this as a domain.
MyDom : with
{
   foo: % -> %;
   coerce: SingleInteger -> %;
   coerce: % -> SingleInteger;
} == AbDom add; -- The "add" ought to be irrelevent ...


import from MyDom, SingleInteger;
print << ((foo(foo(42::MyDom)))::SingleInteger) << newline;
