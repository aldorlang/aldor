-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- tests that builtins work OK for large/small numbers

--> testrun -l axllib
--> testcomp -O
--> testrun -O -l axllib

#include "axllib.as"

import from SingleInteger, Integer;

a:SingleInteger := 1551877902;    -- smaller than max()@SingleInteger
b:Integer := a::Integer;
c:SingleInteger := b::SingleInteger;

print << "a,b,c = " << a << "," << b << "," << c << newline;

d: SingleInteger := 1073741783;
e: SingleInteger := 1073741789;
print <<  352408689@SingleInteger << " " << mod_*(a mod e, d, e) << newline;
print << 478136107@SingleInteger << " " << mod_+(a mod e, d, e) << newline;

p : Integer == 8589934609 ; -- a prime greater than 2^33 (and less than 2^64)
q : Integer == (p - 1) quo 2 ;
g : IntegerMod p == 2 ;
plusOrMinusOne : IntegerMod p == g^q ;  -- g^((p-1) quo 2) = 1 or -1 mod p
print << plusOrMinusOne << newline ;  -- printed result is 64

