-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Doesn't matter which fortran opt we use
--> testgen c -Nsys=fortran-cmplx-void
#include "axllib"

SI ==> SingleInteger;
SF ==> SingleFloat;
DF ==> DoubleFloat;

import from SI, SF, DF;

SC ==> Complex SF; -- FSComplex;

import {
	x: ((SI, SC) -> DF, SI, (SF, String) -> ()) -> SI;
	y: (SI, SC) -> DF;
} from Foreign Fortran; 

xl1(a : SI, b : SC) : DF ==
{
	print << a << real b << newline;
	99.9;
}

xl2(a : SF, b : String) : () ==
{
	print << a << b << newline;
}

x(xl1, 1, xl2);
print << x(xl1, 1, xl2) << x(xl1, 1, xl2) << newline;
x(y, 1, xl2);
