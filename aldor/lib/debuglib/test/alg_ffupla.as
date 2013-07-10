---------------------------- alg_ffupla.as ---------------------------------
-- Copyright (c) Manuel Bronstein 2002
-- Copyright (c) INRIA 2002, Version 1.0.1
-- Logiciel Algebra (c) INRIA 2002, dans sa version 1.0.1
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	V == Vector FX;
	ARR == PrimitiveArray;
	ELIM == TwoStepFractionFreeGaussElimination(FX, M);
	POPOV == UnivariatePolynomialPopovLinearAlgebra(F, FX, M);
	MF == DenseMatrix F;
}

FiniteFieldUnivariatePolynomialLinearAlgebra(
	F: FiniteField,
	FX:UnivariatePolynomialCategory0 F,
	M: MatrixCategory FX): with {

} == add {

	local solve!(a:M, b:M):(M, M, V) == {
		import from Backsolve(FX, M);
		(p,r,st,d) := rowEchelon!(a, b)$ELIM;
		(psol, den) := backsolve(a,p,st,r,b);
		(a, psol, den);
	}
}
