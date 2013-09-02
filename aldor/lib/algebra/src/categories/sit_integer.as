--------------------------- sit_integer.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

local IntegerMod(Z:IntegerCategory, p:Z):ResidueClassRing(Z, p) == add {
	macro Rep == Z;
	import from Rep;

	0:%				== per 0;
	1:%				== per 1;
	local bound:Z			== prev(p) quo 2;
	local field?:Boolean		== prime? p;
	canonicalUnitNormal?:Boolean	== field?;
	characteristic:Integer		== integer p;
	coerce(r:Integer):%		== modularRep(r::Z);
	modularRep(r:Z):%		== per(r mod p);
	canonicalPreImage(x:%):Z	== rep x;
	(a:%) = (b:%):Boolean		== rep(a) = rep(b);
	(a:%) + (b:%):%			== per mod_+(rep a, rep b, p);
	(a:%) - (b:%):%			== per mod_-(rep a, rep b, p);
	(a:%) * (b:%):%			== per mod_*(rep a, rep b, p);
	- (b:%):%			== per mod_-(rep b, p);
	extree(a:%):ExpressionTree	== extree rep a;

	unitNormal(x:%):(%, %, %) == {
		unit? x => (1, x, per invMod(rep x, p));
		(x, 1, 1);
	}

	reciprocal(a:%):Partial % == {
		import from Partial Z;
		failed?(u := recipMod(rep a, p)) => failed;
		[per retract u];
	}

	symmetricPreImage(x:%):Z == {
		(r := canonicalPreImage x) <= bound => r;
		r - p;
	}

	if prime? p then {
		unit?(a:%):Boolean		== true;
		inv(a:%):%			== per invMod(rep a, p);

		(a:%) ^ (n:Integer):% == {
			import from BinaryPowering(%, Integer);
			n >= 0 => binaryExponentiation(a, n);
			inv binaryExponentiation(a, -n);
		}
	}
	else {
		unit?(a:%):Boolean == one? gcd(rep a, p);

		(a:%) ^ (n:Integer):% == {
			import from BinaryPowering(%, Integer);
			assert(n >= 0);
			binaryExponentiation(a, n);
		}
	}
};

#if ALDOC
\thistype{IntegerCategory}
\History{Manuel Bronstein}{16/11/2004}{added ModularComputation, SourceOfPrimes}
\History{Marc Moreno Maza and Steve Wilson}{22/6/04}
{added CanonicalSimplification}
\History{Manuel Bronstein}{23/5/95}{created}
\Usage{\this: Category}
\Descr{\this~is the category of integer-like rings.}
\begin{exports}
\category{\altype{CharacteristicZero}}\\
\category{\altype{EuclideanDomain}}\\
\category{\altype{IntegerType}}\\
\category{\altype{Parsable}}\\
\category{\altype{Specializable}}\\
\alexp{integer}: & \% $\to$ \altype{Integer} & Conversion to an integer\\
\alexp{rationalReconstruction}:
& \% $\to$ \% $\to$ \altype{Partial} \builtin{Cross}(\%, \%) &
Rational reconstruction\\
\end{exports}
#endif

define IntegerCategory: Category ==
	Join(IntegerType, CharacteristicZero, EuclideanDomain,
		ModularComputation, SourceOfPrimes,
		GeneralExponentCategory, Specializable, Parsable) with {
			binomial: (%, %) -> %;
#if ALDOC
\alpage{binomial}
\Usage{\name(n, m)}
\Signature{(\%,\%)}{\%}
\Params{ {\em n,m} & \% & Integers\\}
\Retval{Returns
$$
{n \choose m} = \frac{n!}{m! (n-m)!}\,.
$$
Returns $0$ if either $m < 0$ or $n < m$.}
#endif
			integer: % -> Integer;
#if ALDOC
\alpage{integer}
\Usage{\name~n}
\Signature{\%}{\altype{Integer}}
\Params{ {\em n} & \% & An integer\\}
\Retval{Returns n as an integer.}
#endif
			rationalReconstruction: % -> % -> Partial Cross(%, %);
#if ALDOC
\alpage{rationalReconstruction}
\Usage{\name~m\\ \name(m)(u)}
\Signature{\%}{\% $\to$ \altype{Partial} \builtin{Cross}(\%, \%)}
\Params{
\emph{m} & \% & A nonzero modulus\\
\emph{u} & \% & An Integer\\
}
\Retval{\name(m)(u) returns either $(a,b)$ such that $a/b = u \pmod m$,
$|a| \le \sqrt{(m-1)/2}$ and $|b| \le \sqrt{(m-1)/2}$,
or \failed~if no such $a,b$ exist.}
\Remarks{The resulting $a$ and $b$ are guaranteed to be unique.}
\alseealso{\alfunc{EuclideanDomain}{rationalReconstruction}}
#endif
	default {
		canonicalUnitNormal?:Boolean	== true;
		euclideanSize(n:%):Integer	== integer abs n;
		relativeSize(n:%):MachineInteger== { zero? n => 0; length n }
		cancel?(x:%, y:%):Boolean	== x >= y;
		cancel(x:%, y:%):%		== x - y;
		cancelIfCan(x:%, y:%):Partial %	== { x < y => failed; [x - y] }
		times(n:Integer, x:%):%		== n * x;

		residueClassRing(p:%):ResidueClassRing(%, p) ==
			IntegerMod(% pretend IntegerCategory, p);

		-- the following is necessary because rem is balanced otherwise
		remainder!(a:%, b:%):%		== a mod b;
			
		cutoff(t:MachineInteger):MachineInteger == {
			t = CUTOFF__KARAMULT => 12;
			-1;
		}

		-- returns (y, u, u^{-1}) s.t. x = u y
		unitNormal(x:%):(%, %, %) == {
			import from MachineInteger;
			sign(x) < 0 => (-x,-1,-1);
			(x, 1, 1)
		};

		-- returns (y, u^{-1} z) s.t. x = u y
		-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
		-- unitNormal(x:%, z:%):(%, %) == {
		unitNormalize(x:%, z:%):(%, %) == {
			import from MachineInteger;
			sign(x) < 0 => (-x,-z);
			(x, z)
		}

		binomial(n:%, m:%):% == {
			m < 0 or n < m => 0;
			if m < n - m then m := n - m;
			assert(m >= n - m);
			n = m => 1;
			denom := factorial(n - m);
			numer:% := 1;
			while m < n repeat numer := times!(numer, m := next m);
			assert zero?(numer rem denom);
			quotient(numer, denom);
		}

		extree(x:%):ExpressionTree == {
			import from ExpressionTreeLeaf;
			extree leaf integer x;
		}

		eval(t:ExpressionTreeLeaf):Partial % == {
			import from Integer;
			machineInteger? t => [machineInteger(t)::Integer::%];
			integer? t => [integer(t)::%];
			failed;
		}

		eval(op:MachineInteger, l:List ExpressionTree):Partial % == {
			import from ParsingTools %;
			evalArith(op, l);
		}

		exactQuotient(x:%, y:%):Partial % == {
			(q, r) := divide(x, y);
			zero? r => [q];
			failed
		}

		specialization(Image:CommutativeRing):_
			PartialFunction(%, Image) == {
			partialFunction((n:%):Boolean +-> true,
					(n:%):Image +-> integer(n)::Image);
		}

		rationalReconstruction(modulus:%):% -> Partial Cross(%, %) == {
			assert(modulus > 1);
			(ignore, n) := nthRoot(prev(modulus) quo 2, 2);
			assert(2*n*n < modulus);
			b := integer n;
			(r:%):Partial Cross(%, %) +->
				rationalReconstruction(r, modulus, b, b);
		}
				
		-- THE NEXT 3 ARE MORE EFFICIENT THAT THE GENERAL DEFAULTS
		unit?(x:%):Boolean == one? x or x = -1;

		mod_+(a:%, b:%, p:%):% == {
			assert(0 <= a); assert(a < p);
			assert(0 <= b); assert(a < b);
			(r := a + b) < p => r;
			r - p;
		}

		mod_-(a:%, b:%, p:%):% == {
			assert(0 <= a); assert(a < p);
			assert(0 <= b); assert(b < p);
			(r := a - b) < 0 => r + p;
			r;
		}

		mod_-(b:%, p:%):% == {
			assert(0 <= b); assert(b < p);
			zero? b => 0;
			p - b;
		}

		recipMod(a: %, b: %):  Partial(%) == {
		    assert (b > 1);
		    a := a mod b;
		    zero? a => failed;
                    local c0, d0, c1, d1: %;
                    (c0, d0) := (a, b);
                    (c1, d1) := (1, 0);
                    while not zero? d0 repeat {
			q := c0 quo d0;
                        (c0, d0) := (d0, c0 - q*d0);
                        (c1, d1) := (d1, c1 - q*d1)
                    }
                    if c0 ~= 1 then return failed;
                    if c1 < 0  then c1 := c1 + b;
		    [c1];
		}

		symmetricMod(c__Z: %, m: %): % == {
		    maxBound: % := (m - 1) quo 2;
		    minBound: % := - maxBound;
		    c__Z := c__Z rem m;
		    if (c__Z > maxBound) then  c__Z := minus!(c__Z, m);
		    while (c__Z < minBound) repeat c__Z := add!(c__Z, m);
		    c__Z;
		}

		prime?(p:%):Partial Boolean == {
			import from MachineInteger;
			import from WordSizedPrimes;
			if p < 0 then p := -p;
			p < (max$MachineInteger)::% => [prime?(machine p)];
			failed
		}

		getPrime():Partial % == {
			import from MachineInteger;
			zero?(p := nextPrime(0)$HalfWordSizePrimes) => failed;
			[p::%];
		}

		nextPrime(p:%):Partial % == {
			import from MachineInteger;
			p >= (maxPrime$HalfWordSizePrimes)::% => failed;
			zero?(q := nextPrime(machine p)$HalfWordSizePrimes) =>
				failed;
			[q::%];
		}
	}
};

extend AldorInteger: IntegerCategory == add {
#if GMP
	-- This assumes that Integer == GmpInteger
	integer(u:%):Integer		== u::Integer;
	coerce(n:Integer):%		== coerce(n)$Integer;
#else
	-- Those 2 assume that Integer == AldorInteger
	integer(u:%):Integer		== u;
	coerce(n:Integer):%		== n;
#endif

	-- TEMPORARY: THOSE 6 DEFAULTS ARE NOT TAKEN FROM IntegerCategory!!!
	canonicalUnitNormal?:Boolean	== true;
	relativeSize(n:%):MachineInteger== { zero? n => 0; length n }
	unit?(x:%):Boolean		== one? x or x = -1;
	unitNormal(x:%):(%, %, %) == {
		import from MachineInteger;
		sign(x) < 0 => (-x,-1,-1);
		(x, 1, 1)
	};
	cutoff(t:MachineInteger):MachineInteger == {
		t = CUTOFF__KARAMULT => 12;
		-1;
	}

	-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
	-- unitNormal(x:%, z:%):(%, %) == {
	unitNormalize(x:%, z:%):(%, %) == {
		import from MachineInteger;
		sign(x) < 0 => (-x,-z);
		(x, z)
	}

	-- TEMPORARY: THOSE CANNOT BE DEFINED AS DEFAULTS
	-- AS LONG AS THE COMPILER DOES EARLY-BINDING IN OTHER DEFAULTS
	divide!(a:%, b:%, q:%):(%, %)	== divide(a, b);
	remainder!(a:%, b:%):%		== a rem b;

        getPrimeOfSize(d: MachineInteger): Partial % == failed;
}

extend SingleFloat:Join(Parsable, ExpressionType) == add {
	extree(x:%):ExpressionTree == {
		import from ExpressionTreeLeaf;
		extree leaf(x);
	}

	eval(t:ExpressionTreeLeaf):Partial % == {
		singleFloat? t => [singleFloat t];
		failed;
	}

	eval(op:MachineInteger, args:List ExpressionTree):Partial % == {
		import from ParsingTools %;
		evalArith(op, args);
	}
}

extend DoubleFloat:Join(Parsable, ExpressionType) == add {
	extree(x:%):ExpressionTree == {
		import from ExpressionTreeLeaf;
		extree leaf(x);
		-- extree leaf x;
	}

	eval(t:ExpressionTreeLeaf):Partial % == {
		doubleFloat? t => [doubleFloat t];
		failed;
	}
	
	eval(op:MachineInteger, args:List ExpressionTree):Partial % == {
		import from ParsingTools %;
		evalArith(op, args);
	}
}

#if GMP
extend GMPFloat:Join(Parsable, ExpressionType) == add {
	extree(x:%):ExpressionTree == {
		import from ExpressionTreeLeaf;
		extree leaf(x);
	}

	eval(t:ExpressionTreeLeaf):Partial % == {
		float? t => [float t];
		failed;
	}
	
	eval(op:MachineInteger, args:List ExpressionTree):Partial % == {
		import from ParsingTools %;
		evalArith(op, args);
	}
}
#endif

