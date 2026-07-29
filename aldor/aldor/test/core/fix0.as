#include "foamlib"
#pile


MyList(X: with): with {
--	first: % -> X;
--	rest:  % -> %;
	cons: (X, %) -> %;
	make:  X -> %;
	export from X;
} == add {
	NoValue ==> MachineInteger;
	noValue ==> 0$MachineInteger;
	U == Union(n: NoValue, r: Rep);
	Rep == Record(a: X, u: U);

	import from Rep;

	cons(z:X , x: %): % == per([z, [rep x]]);
	make(z: X): 	  % == per([z, [noValue]]);
}

