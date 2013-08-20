#include "foamlib"

X ==> 'GOOD,BAD,UGLY';	

foo(): X == {
       import from X;
       x := GOOD;
       x;
}

bar(): X == BAD;

bad?(x: X): Boolean == (x = BAD);

print << "hello" << newline
