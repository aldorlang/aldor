#include "foamlib"
#include "assertlib"

X ==> 'GOOD,BAD,UGLY';	

foo(): X == {
       import from X;
       x := GOOD;
       x;
}

bar(): X == BAD;

bad?(x: X): Boolean == (x = BAD);

import from GeneralAssert, X;
assertTrue(bad? BAD);
assertFalse(bad? GOOD);



