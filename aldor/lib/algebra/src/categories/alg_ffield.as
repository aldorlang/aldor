------------------------------- alg_ffield.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FiniteField}
\History{Manuel Bronstein}{17/11/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of finite fields.}
\begin{exports}
\category{\altype{Field}}\\
\category{\altype{FiniteCharacteristic}}\\
\category{\altype{FiniteSet}}\\
\alexp{degree}: & \altype{Integer} & Dimension over the prime field\\
\alexp{pthRoot}: & \% $\to$ \% & Exponentiation to 1 over the characteristic\\
\alexp{pthRoot!}:
& \% $\to$ \% & In-place exponentiation to 1 over the characteristic\\
\end{exports}
#endif

define FiniteField:Category == Join(Field,FiniteCharacteristic,FiniteSet) with {
	degree: Integer;
#if ALDOC
\alpage{degree}
\Usage{\name}
\alconstant{\altype{Integer}}
\Retval{Returns the extension degree of the field, \ie~its dimension
as a vector space over its prime fields. In other words, the size
of the field is $p^\name$ where {\em p} is the characteristic.}
#endif
	pthRoot: % -> %;
	pthRoot!: % -> %;
#if ALDOC
\alpage{pthRoot}
\altarget{\name!}
\Usage{\name~x\\ \name!~x}
\Signature{\%}{\%}
\Params{ {\em x} & \% & An element of the ring\\ }
\Retval{Return {\em y} such that $y^p = x$ where $p$ is the characteristic
of the field.}
\Remarks{\name!~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := pthPower!~x}.}
#endif
	default {

		(a:%)^(n:Integer):% == {
			import from PthPowering %;
			n > 0 => pExponentiation(a, n);
			inv pExponentiation(a, -n);
		}

		-- if size is p^d, then (x^(p^(d-1)))^p = x
		pthRoot(x:%):% == {
			import from Integer;
			% has CopyableType => {
				one? degree => x;
				pthRoot!(copy(x));
			}
			for i in 1..prev(degree) repeat x := pthPower! x;
			x;
		}

		-- if size is p^d, then (x^(p^(d-1)))^p = x
		pthRoot!(x:%):% == {
			import from Integer;
			for i in 1..prev(degree) repeat x := pthPower! x;
			x;
		}
	}
}

