--
-- sieve.as: A prime number sieve to count primes <= n.
-- Usage: Run the code "printSieve(4)" in the REPL below.
--
#include "aldor"
#include "aldorio"

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

printSieve(m: MachineInteger):() == {
	for i in 1..m repeat {
		n  := 10^i;
		stdout << "There are " << sieve n << " primes <= " << n;
		stdout << newline;
	}
}
