------------------------------ sal_bool.as ----------------------------------
--
-- This file extends Boolean to its final salli category
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

-- Boolean is extended in this file, so don't import it
#assert DoNotImportBoolean

#include "aldor"

#if ALDOC
\thistype{Boolean}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this}
\Descr{\this~implements the boolean values \true and \false.}
\begin{exports}
\category{\altype{BooleanArithmeticType}}\\
\category{\altype{HashType}}\\
\alexp{false}: & \% & \false\\
\alexp{true}: & \% & \true\\
\end{exports}

\alpage{true,false}
\altarget{true}
\altarget{false}
\Usage{true\\false}
\alconstant{\%}
\Retval{true and false return the boolean values \true and \false
respectively.}
#endif

extend Boolean:Join(BooleanArithmeticType, HashType) == add {
	import from Machine;
	Rep == Bool;

	(a:%) /\ (b:%):%		== per(rep a /\ rep b);
	(a:%) \/ (b:%):%		== per(rep a \/ rep b);
	hash(a:%):MachineInteger	== { a => 1; 0 }

	-- TEMPORARY (BUG1182) DEFAULTS DON'T INLINE WELL
	xor(a:%, b:%):% == (a /\ ~b) \/ (~a /\ b);
}

#if ALDOC
\thistype{BooleanArithmeticType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types allowing boolean arithmetic.}
\begin{exports}
\category{\altype{PrimitiveType}}\\
\alalias{\this}{not}{$\sim$}: & \% $\to$ \% & negation\\
\alalias{\this}{and}{$\wedge$}: & (\%, \%) $\to$ \% & and\\
\alalias{\this}{or}{$\vee$}: & (\%, \%) $\to$ \% & or\\
\alexp{xor}: & (\%, \%) $\to$ \% & exclusive or\\
\end{exports}
#endif

define BooleanArithmeticType:Category == PrimitiveType with {
	~: % -> %;
	/\: (%, %) -> %;
	\/: (%, %) -> %;
	xor: (%, %) -> %;
#if ALDOC
\alpage{and,or,not,xor}
\altarget{and}
\altarget{or}
\altarget{not}
\altarget{xor}
\Usage{$\sim$ a\\a $\wedge$ b\\a $\vee$ b\\xor(a, b)\\}
\Signatures{
$\sim$: & \% $\to$ \%\\
$\wedge,\vee$,xor: & (\%,\%) $\to$ \%\\
}
\Params{ {\em a, b} & \% & elements of the type\\}
\Retval{$\sim a$ returns $not(a)$, while $a \vee b$ returns
$(a~or~b)$, $a \wedge b$ returns $(a~and~b)$,
and $\mbox{xor}(a, b)$ returns
$$
(a \wedge \sim b) \vee (~\sim a \wedge b)\,.
$$
The semantics of $not$, $or$ and $and$ can be logical or bitwise,
depending on the actual type.}
\Remarks{For the type \altype{Boolean}, the difference between
$a \vee b$ and {\tt a or b} is that $a \vee b$ guarantees
that both expressions $a$ and $b$ are evaluated while {\tt a or b} may
evaluate only $a$ and return \true if $a$ evaluates to \true. There is a
similar difference between $a \wedge b$ and {\tt a and b}.}
\begin{asex}
If a and b are the \altype{MachineInteger} 5 and 7, then
~$\sim a = -6$, $a \wedge b = 5$, $a \vee b = 7$ and
$\mbox{xor}(a,b) = 2$.
\end{asex}
#endif
	default xor(a:%, b:%):% == (a /\ ~b) \/ (~a /\ b);
}

