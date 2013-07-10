
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Tests error reporting for function bodies with no meaning (bug 1144).

--> testerrs

#include "axllib"


S ==> SingleInteger;

LazyS:with
{
    foo: % -> %;
}
== add
{
    Rep ==> Generator S;
    import from Rep;

    foo(a:%):% == 
    {
        ga:Generator S := rep a;
        g:Generator S := generate { repeat yield value(rep a); }
	-- Missing a return value of type % ...
    }
}

