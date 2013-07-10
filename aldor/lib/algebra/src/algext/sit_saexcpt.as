----------------------------- sit_saexcpt.as --------------------------------
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{ReducibleModulusException}
\History{Manuel Bronstein}{15/11/99}{created}
\Usage{
throw \this(R, m, a)\\
try \dots catch E in
\{ E has \astype{ReducibleModulusExceptionType} R $=>$ \dots \}
}
\Params{
{\em R} & \astype{CommutativeRing} & A commutative ring\\
{\em m} & R & The modulus\\
{\em a} & R & A proper factor of $m$\\
}
\Descr{\this(R, m, a) is an exception type thrown by inversion modulo
a reducible element of R.}
#endif
ReducibleModulusException(R:CommutativeRing, m:R, a:R):
	ReducibleModulusExceptionType R == add {
		modulus:R == m;
		factor:R == a;
}

#if ALDOC
\thistype{ReducibleModulusExceptionType}
\History{Manuel Bronstein}{15/11/99}{created}
\Usage{\this~R: Category}
\Params{ {\em R} & \astype{CommutativeRing} & A commutative ring\\ }
\Descr{\this~R~is the category of exceptions thrown by inversion modulo
a reducible element of R. The constants {\tt modulus} and {\tt factor}
contain the modulus and a proper factor respectively.}
#endif
define ReducibleModulusExceptionType(R:CommutativeRing):Category == with {
	modulus: R;
	factor: R;
}

