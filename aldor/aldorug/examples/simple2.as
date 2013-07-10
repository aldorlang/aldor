#include "aldor"

-- Compute a square root by six steps of Newton's method.
-- This gives 17 correct digits for numbers between 1 and 10.

DF ==> DoubleFloat;

miniSqrt(x: DF): DF == {
        r := x;
        r := (r*r + x)/(2.0*r);
        r := (r*r + x)/(2.0*r);
        r := (r*r + x)/(2.0*r);
        r := (r*r + x)/(2.0*r);
        r := (r*r + x)/(2.0*r);
        r := (r*r + x)/(2.0*r);
        r
}
