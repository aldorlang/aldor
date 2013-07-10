--
-- aside.as: An Aldor function made available to C.
--
#include "aldor"

MI ==> MachineInteger;

export { lcm: (MI, MI) -> MI } to Foreign C;

lcm(n: MI, m: MI): MI == (n quo gcd(n,m)) * m;
