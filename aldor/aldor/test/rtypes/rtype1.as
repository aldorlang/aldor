#include "foamlib"
#pile

Z ==> MachineInteger
import from Z;

Foo: with
   f: Z -> Z
== add
   f(x: Z) == x + 1


Bar: with
   f: Z -> Z
   f2: String -> Z
== add
   f(x: Z) == x + 1
   f2(x: String) == 1
