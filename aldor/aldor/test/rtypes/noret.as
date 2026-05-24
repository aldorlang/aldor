#include "foamlib"
#pile

Z ==> MachineInteger
import from Z;

--f(a: ?): Z == return a
--g(a: ?): Z == a

foo(): Z ==
    g == (a: Z): Z +-> a
    g 0
