#include "foamlib"

t1(): SingleInteger == 12;

t2(n: SingleInteger): SingleInteger == n + 1;
t3(a: SingleInteger, b: SingleInteger): (SingleInteger, SingleInteger) == (a+b, a-b);

t4(a: SingleInteger): SingleInteger -> SingleInteger == (x: SingleInteger): SingleInteger +-> a+x;

t5(f: (SingleInteger -> (SingleInteger, SingleInteger)))(n: SingleInteger): SingleInteger == { (x, y) := f n; x+y}
