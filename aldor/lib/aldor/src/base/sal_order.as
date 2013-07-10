----------------------------- sal_order.as ----------------------------------
--
-- This file defines partially ordered types
--
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, and UWO, University of Lille I, 2001
-- Copyright: Marc Moreno Maza
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{PartiallyOrderedType}
\History{Marc Moreno Maza}{23/5/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of partially ordered types.
Hence, a domain of this category is endowed with a transitive
binary relation \alexp{$>$} such that either $x > y$ or $y > x$
does not hold for every {\em x} and {\em y}. If both do not hold
then {\em x} and {\em y} are called not comparable for \alexp{$>$}.
In particular if $x = y$ holds then {\em x} and {\em y} are not
omparable for \alexp{$>$}.}
\begin{exports}
\category{\altype{PrimitiveType}}\\
\alexp{$<$}: & (\%, \%) $\to$ \altype{Boolean} & stricly less than\\
\alexp{$>$}: & (\%, \%) $\to$ \altype{Boolean} & stricly greater than\\
\alalias{\this}{$<$}{$\le$}:
& (\%, \%) $\to$ \altype{Boolean} & less than or equal to\\
\alalias{\this}{$<$}{$\ge$}:
& (\%, \%) $\to$ \altype{Boolean} & greater than or equal to\\
\end{exports}
#endif

define PartiallyOrderedType:Category == PrimitiveType with {
	<: (%, %) -> Boolean;
	>: (%, %) -> Boolean;
	<=: (%, %) -> Boolean;
	>=: (%, %) -> Boolean;
#if ALDOC
\alpage{$<$,$>$}
\altarget{$<$}
\altarget{$>$}
\Usage{$a < b$\\$a > b$\\$a \le b$\\$a \ge b$}
\Signature{(\%,\%)}{\altype{Boolean}}
\Params{ {\em a, b} & \% & elements of the type\\}
\Retval{$a < b$, $a > b$, $a \le b$, $a \ge b$ return \true~when
$a$ is respectively stricly smaller than, stricly greater than,
less than or equal to, greater than or equal to $b$.
Observe that if neither $a > b$ nor $a = b$ hold,
this does not imply that $a < b$ holds, since
{\em a} and {\em b} may not be comparable, except if the
type has \altype{TotallyOrderedType}.}
#endif
	default {
		(a:%) > (b:%):Boolean	== ~(a <= b);
		(a:%) >= (b:%):Boolean	== (a = b) or (a > b);
		(a:%) <= (b:%):Boolean	== (a = b) or (a < b);
	}
}

