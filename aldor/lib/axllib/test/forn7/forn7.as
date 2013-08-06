-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c -Nsys=fortran-cmplx-void
--> testcomp -Nsys=fortran-cmplx-void
-- This test file only applies to platforms which support Fortran
-- functions that return a complex value.
#include "axllib"

SF ==> SingleFloat;
SC ==> Complex SF; -- FSComplex;

import from SF;

import {
	x: () -> (SC);
	y: (() -> SC) -> ();
} from Foreign Fortran; 

export {
	xl1: () -> SC;
} to Foreign Fortran;

xl1() : SC ==
{
	complex(99.9, 99.9);
}

xl2(a : SC) : () ==
{
	print << real a << newline;
}

local a : SC;

x();
a := x();
print << real a << newline;
xl2(x());
y(xl1); 
y(x);
