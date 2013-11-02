#include "aldor"
#include "aldorio"
#library C1 "cond1.ao"
import from C1;

X: with {
   Q;
    coerce: Integer -> %;
    int: % -> Integer;
}
== add {
    Rep == Integer;
    coerce(n: Integer): % == per(n+1);
    int(x: %): Integer == rep x;
}

test(): () == {
   import from Seg X;
   import from Integer;
   import from X;
   stdout << "This really should be 13: " << int(foo 12) << newline;
}

test();
