---------------------- sit_pring.as -----------------------------
--
-- Partial rings, i.e. rings where operations are allowed to fail
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	Z	== Integer;
	U	== Partial %;
}

#if ALDOC
\thistype{PartialRing}
\History{Manuel Bronstein}{10/01/96}{created}
\Usage{\this: Category}
\Descr{\this~is the category of rings where all the arithmetic operations
are partial, \ie~they are allowed to fail. Typical examples are matrices
of different sizes, or unions of several true rings.}
\begin{exports}
\category{\altype{ExpressionType}}\\
\alexp{0}: & \% & Additive identity\\
\alexp{1}: & \% & Multiplicative identity \\
\alexp{-}: &  \% $\to$ \altype{Partial} \% & Negation \\
\alexp{-}: &  (\%, \%) $\to$ \altype{Partial} \% & Substraction \\
\alexp{+}: &  (\%, \%) $\to$ \altype{Partial} \% & Addition \\
\alexp{*}: &  (\%, \%) $\to$ \altype{Partial} \% & Multiplication \\
\alexp{/}: &  (\%, \%) $\to$ \altype{Partial} \% & Exact division \\
\alexp{$\land$}: &  (\%, \%) $\to$ \altype{Partial} \% & Exponentiation \\
\alexp{<}: &  (\%, \%) $\to$ \altype{Partial} \% & Comparison \\
\alexp{>}: &  (\%, \%) $\to$ \altype{Partial} \% & Comparison \\
\alexp{$\le$}: &  (\%, \%) $\to$ \altype{Partial} \% & Comparison \\
\alexp{$\ge$}: &  (\%, \%) $\to$ \altype{Partial} \% & Comparison \\
\alexp{$[]$}:
& \builtin{Tuple} \% $\to$ \altype{Partial} \% & Construct a structure\\
\alexp{coerce}:
& \altype{Boolean} $\to$ \% & Convert a boolean to a ring element\\
\alexp{coerce}:
&  \altype{Integer} $\to$ \% & Convert an integer to a ring element\\
\alexp{integer}:
& \% $\to$ \altype{Partial} \altype{Integer} & Convert to an integer\\
\alexp{product}: &  List \% $\to$ \altype{Partial} \% & Multiplication \\
\alexp{sum}: &  List \% $\to$ \altype{Partial} \% & Addition \\
\end{exports}
#endif

define PartialRing: Category == ExpressionType with {
	0:%;
	1:%;
	-: % -> U;
	-: (%, %) -> U;
	+: (%, %) -> U;
	*: (%, %) -> U;
	/: (%, %) -> U;
	^: (%, %) -> U;
	<: (%, %) -> U;
	>: (%, %) -> U;
	<=: (%, %) -> U;
	>=: (%, %) -> U;
	coerce: Boolean -> %;
	coerce: Z -> %;
	integer: % -> Partial Z;
	list: List % -> U;
	product: List % -> U;
	sum: List % -> U;
	default {
		0:%			== { import from Z; 0 :: % }
		1:%			== { import from Z; 1 :: % }
		sum(l:List %):U		== partialReduce(+, l, 0);
		product(l:List %):U	== partialReduce(*, l, 1);
		coerce(b?:Boolean):%	== { b? => 1; 0 }
		list(l:List %):U	== failed;

		(a:%) <= (b:%):U == {
			import from Boolean;
			a = b => [true::%];
			a < b;
		}

		(a:%) >= (b:%):U == {
			import from Boolean;
			a = b => [true::%];
			a > b;
		}

		(a:%) > (b:%):U == {
			import from Boolean;
			a = b => [false::%];
			failed?(u := a < b) => u;
			[(0 = retract u)::%];
		}

		-- default is that only the integers are ordered
		(a:%) < (b:%):U == {
			import from Boolean, Z, U, Partial Z;
			a = b => [false::%];
			failed?(u := integer a)
				or failed?(v := integer b) => failed;
			[(retract u < retract v)::%];
		}

		local partialReduce(op: (%, %) -> U, l:List %, def:%):U == {
			import from Boolean;
			empty? l => [def];
			x := first l;
			while ~empty?(l := rest l) repeat {
				failed?(u := op(x, first l)) => return failed;
				x := retract u;
			}
			[x];
		}

		(a:%) - (b:%):U == {
			failed?(u := -b) => u;
			a + retract u;
		}
	}
}
