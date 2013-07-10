--------------------------- sit_prfcat0.as --------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{PrimeFieldCategory0}
\History{Manuel Bronstein}{16/6/95}{created}
\Usage{\this: Category}
\Descr{\this~is the category for prime fields,\ie fields of the form
$\ZZ / p \ZZ$ where $p \in \ZZ$ is a prime.}
\begin{exports}
\category{\altype{FiniteField}}\\
\category{\altype{SerializableType}}\\
\alexp{lift}: & \% $\to$ \altype{Integer} & Conversion to an integer\\
\end{exports}
#endif

define PrimeFieldCategory0:Category == Join(FiniteField,SerializableType) with {
    lift: % -> Integer;

#if ALDOC
\alpage{lift}
\Usage{\name~x}
\Signature{\%}{\altype{Integer}}
\Params{ {\em x} & \% & an element of the field\\ }
\Retval{Return x seen as an integer.}
#endif
		default {
			degree:Integer		== 1;
			pthPower(a:%):%		== a;
			pthPower!(a:%):%	== a;
			pthRoot(a:%):%		== a;
			pthRoot!(a:%):%		== a;
			index(a:%):Integer	== lift a;

			(port:BinaryWriter) << (a:%):BinaryWriter == {
				import from Integer;
				port << lift a;
			}

			<< (port:BinaryReader):% == {
				import from Integer;
				(<< port)@Integer :: %;
			}

			lookup(n:Integer):% == {
				assert(n >= 0); -- assert(n < #$%); BUG 1181
				n::%;
			}

			-- TEMPORARY: BUG 1181
			-- #:Integer		== characteristic$%;

			extree(x:%):ExpressionTree == { 
				import from Integer;
				extree lift x;
			}
		}
}
