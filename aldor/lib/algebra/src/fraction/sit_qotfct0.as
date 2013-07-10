------------------------ sit_qotfct0.as ---------------------------
-- Copyright (c) Laurent Bernardin 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{FractionFieldCategory0}
\History{Laurent Bernardin}{1/12/94}{created}
\History{Manuel Bronstein}{25/5/95}
{extracted the category from the type in order to create RationalCategory}
\History{Manuel Bronstein}{21/9/95}{added UnivariateGcdRing and gcd lifting}
\Usage{\this~R: Category}
\Params{ {\em R} & \altype{IntegralDomain} & an integral domain\\ }
\Descr{\this~R is the category of the fraction fields of R.}
\begin{exports}
\category{\altype{Field}}\\
\category{\altype{FractionCategory} R}\\
\alexp{/}: & (R,R) $\to$ \% & Quotient of two ring elements\\
\end{exports}
\begin{exports}[if {\em R} has \altype{CharacteristicZero} then]
\category{\altype{RittRing}}\\
\end{exports}
\begin{exports}[if {\em R} has \altype{Parsable} then]
\category{\altype{Parsable}}\\
\end{exports}
\begin{exports}[if {\em R} has \altype{SerializableType} then]
\category{\altype{SerializableType}}\\
\end{exports}
\begin{exports}[if {\em R} has \altype{TotallyOrderedType} then]
\category{\altype{TotallyOrderedType}}\\
\end{exports}
#endif

define FractionFieldCategory0(R: IntegralDomain): Category ==
	Join(Field, FractionCategory R) with {
	if R has CharacteristicZero then RittRing;
	if R has SerializableType then SerializableType;
	if R has TotallyOrderedType then TotallyOrderedType;
	if R has Parsable then Parsable;
	/:		(R, R) -> %;
#if ALDOC
\alpage{/}
\Usage{n~\name~d}
\Signature{(R,R)}{\%}
\Params{
{\em n} & R & An element of the ring.\\
{\em d} & R & A nonzero element of the ring.\\
}
\Retval{Returns the quotient {\em n} over {\em d}.}
#endif
	default {
		import from R;

		(p:R) * (a:%):% == (p * numerator a) / (denominator a);
		random():%	== random()$R / random()$R;

		relativeSize(a:%):MachineInteger ==
			relativeSize(numerator a) + relativeSize(denominator a);

		if R has CharacteristicZero then inv(n:Integer):% == 1 / (n::R);

		(a:%) + (b:%):% == {
			(numerator a * denominator b +
				denominator a * numerator b) /
					(denominator a * denominator b);
		}
	
		(a:%) * (b:%):% == {
			(numerator a * numerator b) /
				(denominator a * denominator b);
		}

		(a:%) = (b:%):Boolean == {
			numerator a * denominator b =
				denominator a * numerator b;
		}

		if R has TotallyOrderedType then {
			(a:%) < (b:%):Boolean == {
				numerator(a) * denominator(b) <
					numerator(b) * denominator(a);
			}
		}

		lift(D:Derivation R):Derivation % == {
			import from Integer;
			derivation {(f:%):% +-> {
				n := numerator f; d := denominator f;
				D(n) / d - n * (D(d) / d^2);
			} }
		}

		if R has FiniteCharacteristic then {
			pthPower(a:%):% ==
				pthPower(numerator a) / pthPower(denominator a);
		}

		if R has SerializableType then {
			(port:BinaryWriter) << (a:%):BinaryWriter == {
				import from R;
				port << numerator a << denominator a;
			}

			<< (port:BinaryReader):% == {
				n:R := << port;
				n / (<< port)@R;
			}
		}

		if R has Parsable then {
			eval(t:ExpressionTreeLeaf):Partial % == {
				import from Partial R;
				failed?(u := eval(t)$R) => failed;
				[retract(u)::%];
			}

			eval(op:MachineInteger,
				args:List ExpressionTree):Partial % == {
					import from ParsingTools %;
					evalArith(op, args);
			}
		}
	}
}		
