--------------------------- alg_pf2.as --------------------------------
-- Copyright (c) Julien Ohler 2002
-- Copyright (c) INRIA 2002, Version 1.0.1
-- Logiciel Algebra (c) INRIA 2002, dans sa version 1.0.1
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{PrimeField2}
\History{Julien Ohler}{17/06/2002}{created}
\Usage{ import from \this }
\Descr{\this implements the finite field with two elements.}
\begin{exports}
\category{\altype{SmallPrimeFieldCategory}}\\
\end{exports}
#endif

PrimeField2:SmallPrimeFieldCategory == add {
	Rep == MachineInteger;
	import from Rep;

	(a:%)=(b:%):Boolean == {rep a = rep b}
	0:% == per 0;
	1:% == per 1;
	-(a:%):% == a;
	(a:%)*(b:%):% == per (rep a /\ rep b);
	(a:%)+(b:%):% == per xor(rep a,rep b);
        (a:%)-(b:%):% == a+b;
	(a:%)/(b:%):% == { assert(b=1); a;}
	inv(a:%): % == {assert(a=1);1;} 

	characteristic:Integer == 2;
	#:Integer == 2;

	coerce(n:Integer):% == {bit?(n,0)=>1;0} 	--TEMPORARY: BUG 1338
	coerce(n:MachineInteger):% == {bit?(n,0)=>1;0}	--TEMPORARY: BUG 1338
	lift(a:%):Integer == {a=0=>0;1}
}


