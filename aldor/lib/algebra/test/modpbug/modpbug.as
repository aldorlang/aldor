------------------------ modpbug.as ------------------------------
--
-- Yet another optimizer bug:
-- % aldor -q1 -fx -lalgebra -laldor modpbug.as
-- % modpbug
-- causes a segmentation fault.
-- The bug disappears if algebra/src/mat/modular/sit_modpoge.as
-- is recompiled at -q1 and linked with modbug
-- It reappears at -q2 and higher
-- But the above file is critical C-like code that must be optimized.
--

#include "algebra"
#include "aldorio"

macro {
	I == MachineInteger;
	Z == Integer;
	F == SmallPrimeField 3;
	M == DenseMatrix;
}

import from Symbol, I;

local bug():() == {
	import from F, LinearAlgebra(F, M F);
	m:M F := zero(8,3);
	m(1,2) := 1;
	m(2,3) := 1;
	stdout << m << endnl;
	K := kernel m;
	stdout << K << endnl;
}

bug();
