--> testrun -O -l axllib 
--> testcomp -O

#include "axllib"

set!(x: Integer, y: Integer, z: Integer): (Integer, Integer) == (x+y, x-y);
apply(x: Integer, y: Integer): (Integer, Integer) == (x+y, x-y);
f(a: Integer, b: Integer): Integer == a+b;
import from Integer;
x := f(12 . 2);
print << x << newline;

x := f(12 . 2 := 3);
print << x << newline;
