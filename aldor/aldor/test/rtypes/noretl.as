#include "foamlib"
#pile

Z ==> MachineInteger
import from Z;

accept1(f: () -> Z): Z == f()

accept1( (): ? +-> 1)

accept2(g: Z -> Z): Z == g(1)

accept2( (a: ?): ? +-> 1)

accept2( (a: ?): ? +-> a)

