------------------------------- sit_charp.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-------------------------------------------------------------------------------

#include "algebra"

macro {
	MZ == MachineInteger;
	Z  == Integer;
}

#include "sit_power"

#if ALDOC
\thistype{FiniteCharacteristic}
\History{Manuel Bronstein}{22/11/94}{created}
\Usage{\this: Category}
\Descr{\this~is the category of finite characteristic rings.}
\begin{exports}
\category{\altype{Ring}}\\
\alexp{pthPower}: & \% $\to$ \% & Exponentiation to the characteristic\\
\alexp{pthPower!}:
& \% $\to$ \% & In--place exponentiation to the characteristic\\
\end{exports}
#endif

define FiniteCharacteristic: Category == Ring with {
	pthPower: % -> %;
	pthPower!: % -> %;
#if ALDOC
\alpage{pthPower}
\altarget{\name!}
\Usage{\name~x\\ \name!~x}
\Signature{\%}{\%}
\Params{ {\em x} & \% & An element of the ring\\ }
\Retval{Return $x^p$ where $p$ is the characteristic of the ring.}
\Remarks{\name!~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := pthPower!~x}.}
#endif
	default {
		pthPower!(a:%):% == pthPower a;

		(a:%)^(n:Integer):% ==
			pExponentiation(a,
				n)$PthPowering(% pretend FiniteCharacteristic);
	}
}
