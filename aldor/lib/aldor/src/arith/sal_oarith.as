------------------------------ sal_oarith.as ---------------------------------
--
-- Types with a total order and the basic arithmetic operations
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{OrderedArithmeticType}
\History{Manuel Bronstein}{26/8/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of ordered types with
the standard arithmetic operations.}
\begin{exports}
\category{\altype{ArithmeticType}}\\
\category{\altype{TotallyOrderedType}}\\
\alexp{abs}: & \% $\to$ \% & norm\\
\alexp{sign}: & \% $\to$ \altype{MachineInteger} & sign\\
\end{exports}
#endif

define OrderedArithmeticType:Category ==
	Join(ArithmeticType, TotallyOrderedType) with {
	abs: % -> %;
#if ALDOC
\alpage{abs}
\Usage{\name~x}
\Signature{\%}{\%}
\Params{{\em x} & \% & an element of the type\\ }
\Retval{Returns the norm $|x|$ of x.}
#endif
	sign: % -> MachineInteger;
#if ALDOC
\alpage{sign}
\Usage{\name~x}
\Signature{\%}{\altype{MachineInteger}}
\Params{{\em x} & \% & an element of the type\\ }
\Retval{Returns $1$ if $x > 0$, $0$ if $x = 0$ and $-1$ if $x < 0$.}
#endif
	default {
		abs(a:%):% == { a < 0 => -a; a }

		compare(a:%, b:%):MachineInteger == {
			sa := sign a; sb := sign b;
			sa = sb => {
				zero? sa => 0;
				sign(a - b);
			}
			sa - sb;
		}

		sign(a:%):MachineInteger == {
			zero? a => 0;
			a < 0 => 0-1;
			1;
		}

	}
}

