-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -l axllib

#include "axllib.as"

INT ==> SingleInteger;

import from INT;

test(u:INT):INT == {
    lineNo := u;
    foo : Array INT := new(3,0);
    print << "lineNo is " << lineNo << newline;
    foo.2 := (lineNo := lineNo+1);  -- Naughty!
    print << "lineNo is " << lineNo << newline;
    print << "foo.2 is " << foo.2 << newline;
    lineNo
}

test(1);


