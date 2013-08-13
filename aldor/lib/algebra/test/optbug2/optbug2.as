------------------------------- optbug.as --------------------------------
--
-- % aldor -fx -lalgebra -laldor optbug.as
-- % optbug
-- Segmentation fault
-- The bug disappears if algebra/src/mat/modular/sit_modpoge.as
-- is recompiled at -q1 and linked with optbug
-- It reappears always when sit_modpoge is compiled at -q3 and higher
-- It appears occasionally with other matrices when compiled -q2
-- But sit_modpoge is critical C-like code that must be optimized
-- down to machine integer operations

#include "algebra"
#include "aldorio"

macro {
        Z == Integer;
	F == SmallPrimeField 3;
        FX == DenseUnivariatePolynomial(F, -"x");
	M == DenseMatrix;
}


main():() == {
	import from Symbol, MachineInteger, F, FX;
	import from LinearAlgebra(FX, M FX);

	m:M FX := zero(6,6);
	m(1,2) := 1; m(1,5) := -1;
	m(2,3) := -1; m(2,6) := -1;
	m(4,5) := 1;
	m(5,6) := -1;
	r := rank m;
	stdout << "rank(m) = " << r << newline;
}

main();

