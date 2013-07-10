------------------------------ sit_comring.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{CommutativeRing}
\History{Manuel Bronstein}{22/11/94}{created}
\History{Manuel Bronstein}{24/09/96}{added exactFactors}
\History{Manuel Bronstein}{6/12/96}{added karatsubaCutoff}
\History{Manuel Bronstein}{6/6/2002}{moved quotients to IntegralDomain}
\Usage{\this: Category}
\Descr{\this~is the category of commutative rings.}
\begin{exports}
\category{\altype{Ring}}\\
\alexp{canonicalUnitNormal?}: & $\to$ \altype{Boolean} &
Check if \alexp{unitNormal} is canonical\\
\alexp{cutoff}: & Z $\to$ Z & Cutoffs for various fast algorithms\\
\alexp{reciprocal}: & \% $\to$ \altype{Partial} \% & Inverse\\
\alexp{unitNormal}:
& \% $\to$ (\%,\%,\%) & Representative of the associates\\
& (\%, \%) $\to$ (\%,\%) & \\
\alexp{unit?}: & \% $\to$ \altype{Boolean} & Test whether an element is a unit\\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

define CommutativeRing: Category == Ring with {
	canonicalUnitNormal?: Boolean;
#if ALDOC
\alpage{canonicalUnitNormal?}
\Usage{\name}
\alconstant{\altype{Boolean}}
\Retval{Returns \true~if {\tt unitNormal} is canonical in the following sense:
if $x = v x'$ for some unit $v$ and ${\tt unitNormal}(x)$ returns
$(y, u, u^{-1})$ and ${\tt unitNormal}(x')$ returns $(y', u', u'{}^{-1})$,
then $y = y'$.}
\alseealso{\alexp{unitNormal}, \alexp{unit?}}
#endif
	cutoff: MachineInteger -> MachineInteger;
#if ALDOC
\alpage{cutoff}
\Usage{\name~t}
\Signature{\altype{MachineInteger}}{\altype{MachineInteger}}
\Retval{Returns various cutoffs for asymptotically fast algorithms,
which are then used for structures (\eg~polynomials or matrices) over
this ring when the input size is greater than the corresponding cutoff.
The parameter {\em t} denotes the algorithm in question and must be one
of the CUTOFF\_\_XXX values defined in {\tt include/algebrauid.as}}
\Remarks{If a cutoff is $-1$, then the corresponding algorithm is not
used at all over this ring. The default value is always $-1$ so you only
need to define other values if you want particular algorithms to be used
over your rings.}
#endif
	reciprocal: % -> Partial %;
#if ALDOC
\alpage{reciprocal}
\Usage{\name~x}
\Signature{\%}{\altype{Partial} \%}
\Params{ {\em x} & \% & An element of the ring\\ }
\Retval{Returns the unique \emph{y} such that $x\, y = 1$ if such a
\emph{y} exists, \failed otherwise.}
#endif
	unitNormal: % -> (%, %, %);
	-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
	-- unitNormal: (%, %) -> (%, %);
	unitNormalize: (%, %) -> (%, %);
#if ALDOC
\alpage{unitNormal}
\Usage{(y, u, u1) := \name x\\ (y, z1) := \name(x, z)}
\Signatures{
\name: \% $\to$ (\%, \%. \%)\\
\name: (\%,\%) $\to$ (\%. \%)\\
}
\Params{ {\em x,y} & \% & Elements of the ring\\ }
\Retval{ \name(x) returns $(y, u, u^{-1})$, while
\name(x,z) returns $(y, u^{-1} z)$. In both cases, $x = u y$ and
$u$ is a unit.}
\alseealso{\alexp{canonicalUnitNormal?}, \alexp{unit?}}
#endif
	unit?: % -> Boolean;
#if ALDOC
\alpage{unit?}
\Usage{\name~x}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em x} & \% & An element of the ring\\ }
\Retval{ Returns \true~if $x$ is a unit, \ie $x \, y = 1$ for some $y$,
\false~otherwise.}
\alseealso{\alexp{canonicalUnitNormal?}, \alexp{unitNormal}}
#endif
	default {
		canonicalUnitNormal?:Boolean		== false;
		commutative?:Boolean			== true;
		unitNormal(x:%):(%, %, %)		== (x, 1, 1);
		cutoff(t:MachineInteger):MachineInteger	== -1;

		-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
		-- unitNormal(x:%, z:%):(%, %) == {
		unitNormalize(x:%, z:%):(%, %) == {
			(y, u, uinv) := unitNormal x;	-- x = u y
			(y, uinv * z);
		}

		unit?(x:%):Boolean == {
			import from Partial %;
			~(zero? x or failed? reciprocal x)
		}
	}
}
