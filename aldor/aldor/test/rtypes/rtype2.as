#include "foamlib"
#pile

Z ==> MachineInteger

twice(f: Z -> Z)(x: Z): Z == f f x

--default n: Z
q := twice( (n): Z +-> n + 1)

f1: Z -> Z == (a: Z): Z +-> a+1
f2: Z -> Z == (a): Z +-> a+1
f3: Z -> Z == a: Z +-> a+1
f4: Z -> Z == a +-> a+1

g1: () -> Z == (): Z +-> 1
g2: () -> Z == () +-> 1

h1: (Z, Z) -> Z == (a, b) +-> a+b
h2: (Z, Z) -> Z == (a, b: Z) +-> a+b
h3: (Z, Z) -> Z == (a, b): Z +-> a+b
