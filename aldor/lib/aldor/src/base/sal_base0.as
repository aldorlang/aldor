#assert DoNotImportBoolean

#include "aldor"

import from Machine;

#if ALDOC
\thistype{PrimitiveType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of the most basic types.}
\begin{exports}
\alexp{$=$}: & (\%, \%) $\to$ \altype{Boolean} & equality test\\
\alalias{\this}{$=$}{$\sim=$}:
& (\%, \%) $\to$ \altype{Boolean} & inequality test\\
\end{exports}
#endif

define PrimitiveType: Category == with {
	=: (%, %) -> Boolean;
	~=: (%, %) -> Boolean;
#if ALDOC
\alpage{$=$}
\Usage{a = b\\ a $\sim=$ b}
\Signatures{
$=$: & (\%,\%) $\to$ \altype{Boolean}\\
$\sim=$: & (\%,\%) $\to$ \altype{Boolean}\\
}
\Params{ {\em a, b} & \% & elements of the type\\ }
\Retval{ If $a = b$ returns \true, then $a$ and $b$ are guaranteed to
represent the same element of the type. The behavior if $a = b$ returns
\false~depends on the type, since a full equality test might not be
available. At least, it is guaranteed that $a$ and $b$ do not share the
same memory location in that case. The semantics of $a~\sim= b$ is
the boolean negation of $a = b$.}
#endif
	default { (a:%) ~= (b:%):Boolean == ~(a = b); }
}

+++ The Boolean data type supports logical operations.
+++ Both arguments of the binary operations are always evaluated.
+++ The Boolean type is "magic" for the compiler which expects 
+++ Boolean values for such things as if statements.
Boolean: PrimitiveType with {
	~: % -> %;
	coerce: Bool -> %;
	coerce: % -> Bool;
	false:%;
	true:%;
} == add {
	Rep == Bool;

	coerce(b:%):Bool	== rep b;
	coerce(b:Bool):%	== per b;
	false:%			== false@Bool :: %;
	true:%			== true@Bool :: %;
	~(x:%):%		== (~(x::Bool))::%;
	(a:%) = (b:%):%		== (rep a = rep b)::%;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):%	== (rep a ~= rep b)::%;
}
