--
-- sieve.as: A prime number sieve to count primes <= n.
--
# include "aldor"
# include "aldorio"

import from Boolean, MachineInteger;

sieve(n: MachineInteger): MachineInteger == {
        isprime: PrimitiveArray Boolean := new(n, true);

        np := 0;
        for p in 2..n | isprime p repeat {
                np := np + 1;
                for i in 2*p..n by p repeat isprime i := false;
        }
        np
}

for i in 1..4 repeat {
        n  := 10^i;
        stdout << "There are " << sieve n << " primes <= " << n;
        stdout << newline;
}

doit():Integer==0
