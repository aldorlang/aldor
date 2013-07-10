------------------------------ sal_intcat.as ---------------------------------
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{IntegerType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types representing integers.}
\begin{exports}
\category{\altype{BooleanArithmeticType}}\\
\category{\altype{HashType}}\\
\category{\altype{InputType}}\\
\category{\altype{OrderedArithmeticType}}\\
\category{\altype{OutputType}}\\
\category{\altype{SerializableType}}\\
\alexp{bit?}:
& (\%, \altype{MachineInteger}) $\to$ \astype{Boolean} & check a bit\\
\alexp{clear}: & (\%, \altype{MachineInteger}) $\to$ \% & clear a bit\\
\alexp{coerce}:
& \altype{MachineInteger} $\to$ \% & conversion from machine integer\\
\alexp{divide}: & (\%, \%) $\to$ (\%, \%) & Euclidean division\\
\alexp{even?}: & \% $\to$ \altype{Boolean} & test whether a number is even\\
\alexp{factorial}: & \% $\to$ \% & factorial\\
\alexp{gcd}: & (\%, \%) $\to$ \% & greatest common divisor\\
\alexp{lcm}: & (\%, \%) $\to$ \% & least common multiple\\
\alexp{length}: & \% $\to$ \altype{MachineInteger} & number of bits\\
\alexp{machine}:
& \% $\to$ \altype{MachineInteger} & conversion to a machine integer\\
\alexp{mod}: & (\%, \%) $\to$ \% & remainder\\
             & (\%, \altype{MachineInteger}) $\to$ \astype{MachineInteger} & \\
\alexp{next}: & \% $\to$ \% & next greater integer\\
\alexp{nthRoot}: & (\%, \%) $\to$ (\altype{Boolean}, \%) & $\sth{n}$--root\\
\alexp{odd?}: & \% $\to$ \altype{Boolean} & test whether a number is odd\\
\alexp{prev}: & \% $\to$ \% & next smaller integer\\
\alexp{quo}: & (\%, \%) $\to$ \% & quotient\\
\alexp{random}: & () $\to$ \% & random integer\\
                & \altype{MachineInteger} $\to$ \% & \\
\alexp{rem}: & (\%, \%) $\to$ \% & remainder\\
\alexp{set}: & (\%, \altype{MachineInteger}) $\to$ \% & set a bit\\
\alexp{shift}: & (\%, \altype{MachineInteger}) $\to$ \% & shift\\
\alexp{shift!}: & (\%, \altype{MachineInteger}) $\to$ \% & in--place shift\\
\end{exports}
#endif

define IntegerType:Category ==
	Join(OrderedArithmeticType, BooleanArithmeticType, HashType,
			InputType, OutputType, SerializableType) with {
		bit?: (%, Z) -> Boolean;
		clear: (%, Z) -> %;
		set: (%, Z) -> %;
#if ALDOC
\alpage{bit?,clear,set}
\altarget{bit?}
\altarget{clear}
\altarget{set}
\Usage{bit?(a, n)\\ clear(a, n)\\ set(a, n)}
\Signatures{
bit?: & (\%, \altype{MachineInteger}) $\to$ \astype{Boolean}\\
clear, set: & (\%, \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em a} & \% & an integer\\
{\em n} & \altype{MachineInteger} & a nonnegative machine integer\\
}
\Retval{bit?(a, n) returns \true if the $\sth{n}$ bit of a is 1, \false if it
is 0, while clear(a, n) and set(a, n) return copies of a where the $\sth{n}$ bit
is set respectively to 0 and 1. For all 3 functions,
the rightmost bit of a is the $\sth{0}$ bit and so on.}
#endif
		coerce: Z -> %;
		machine: % -> Z;
#if ALDOC
\alpage{coerce,machine}
\altarget{coerce}
\altarget{machine}
\Usage{n::\%\\ machine~a}
\Signatures{
coerce: & \altype{MachineInteger} $\to$ \%\\
machine: & \% $\to$ \altype{MachineInteger}\\
}
\Params{
{\em a} & \% & an integer\\
{\em n} & \altype{MachineInteger} & a machine integer\\
}
\Retval{n::\% returns n converted to the current type, while machine(a)
returns the low machine word of a converted to a \altype{MachineInteger}.
That operation can cause a loss of precision if a is greater than a machine
word.}
#endif
		divide: (%, %) -> (%, %);
		mod: (%, Z) -> Z;
		mod: (%, %) -> %;
		quo: (%, %) -> %;
		rem: (%, %) -> %;
#if ALDOC
\alpage{divide,mod,quo,rem}
\altarget{divide}
\altarget{mod}
\altarget{quo}
\altarget{rem}
\Usage{divide(a, b)\\ a mod n\\ a mod b\\ a quo b\\ a rem b}
\Signatures{
divide: & (\%,\%) $\to$ (\%, \%)\\
mod,quo,rem: & (\%,\%) $\to$ \%\\
mod: & (\%,\altype{MachineInteger}) $\to$ \astype{MachineInteger}\\
}
\Params{
{\em a,b} & \% & integers, $b \ne 0$\\
{\em n} & \altype{MachineInteger} & a nonzero machine integer\\
}
\Retval{$a$ mod $b$ (resp.~$a$ mod $n$) returns $m$ such that $0 \le m < |b|$
(resp.~$0 \le m < |n|$) and $a \equiv m \pmod b$ (resp $n$),
while $a$ rem $b$ returns $r$ such that $-|b| < r < |b|$ and
$a \equiv r \pmod b$,
$a$ quo $b$ returns $(a - (a$ rem $b)) / b$,
and divide(a, b) returns the pair (a quo b, a rem b).}
\Remarks{mod returns a unique remainder modulo b, but is more expensive
to compute than rem, and is not guaranteed to be compatible with the
result of quo. The version whose second argument is a
\altype{MachineInteger} allows for more efficient implementations.}
#endif
		even?: % -> Boolean;
		odd?: % -> Boolean;
#if ALDOC
\alpage{even?,odd?}
\altarget{even?}
\altarget{odd?}
\Usage{even?~a\\odd?~a}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em a} & \% & an integer\\ }
\Retval{even?(a) and odd?(a) return \true when a is even, respectively odd,
\false otherwise.}
#endif
		factorial: % -> %;
#if ALDOC
\alpage{factorial}
\Usage{\name~a}
\Signature{\%}{\%}
\Params{ {\em a} & \% & a nonnegative integer\\ }
\Retval{Returns $a! = \prod_{i=1}^a i$.}
#endif
		gcd: (%, %) -> %;
		lcm: (%, %) -> %;
#if ALDOC
\alpage{gcd,lcm}
\altarget{gcd}
\altarget{lcm}
\Usage{gcd(a, b)\\ lcm(a, b)}
\Signature{(\%,\%)}{\%}
\Params{ {\em a,b} & \% & integers\\ }
\Retval{gcd(a, b) and lcm(a, b) return respectively a greatest common divisor
and a least common multiple of a and b.}
#endif
		integer: Literal -> %;
		length: % -> Z;
#if ALDOC
\alpage{length}
\Usage{\name~a}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em a} & \% & an integer\\ }
\Retval{Returns the number of binary bits of a, \ie n such that
\alexp{bit?}$(a, n-1)$ is \true and \alexp{bit?}$(a, m)$
is \false for $m \ge n$.}
#endif
		next: % -> %;
		prev: % -> %;
#if ALDOC
\alpage{next,prev}
\altarget{next}
\altarget{prev}
\Usage{next~a\\prev~a}
\Signature{\%}{\%}
\Params{ {\em a} & \% & an integer\\ }
\Retval{next(a) and prev(a) return $a+1$ and $a-1$ respectively.}
#endif
		nthRoot: (%, %) -> (Boolean, %);
#if ALDOC
\alpage{nthRoot}
\Usage{\name(a, b)}
\Signature{(\%,\%)}{(\altype{Boolean},\%)}
\Params{
{\em a} & \% & an integer\\
{\em b} & \% & a positive integer\\
}
\Retval{Returns (found?, n) such that
$a = n^b$ if found?~is \true. Otherwise, found?~is \false~and
$$
n^b < a < (n+1)^b\,.
$$
}
#endif
		random: () -> %;
		random: Z -> %;
#if ALDOC
\alpage{random}
\Usage{\name()\\ \name~n}
\Signatures{
\name: () $\to$ \%\\
\name: \altype{MachineInteger} $\to$ \%\\
}
\Params{ {\em n} & \altype{MachineInteger} & a positive size\\ }
\Retval{\name() returns a random integer, while \name(n) returns
a random integer with $n$ limbs.}
#endif
		shift: (%, Z) -> %;
		shift!:(%, Z) -> %;
#if ALDOC
\alpage{shift}
\altarget{\name!}
\Usage{\name(a, n)\\ \name!(a, n)}
\Signature{(\%, \altype{MachineInteger})}{\%}
\Params{
{\em a} & \% & an integer\\
{\em n} & \altype{MachineInteger} & a machine integer\\
}
\Retval{Returns $a$ shifted left $n$ times if $n \ge 0$,
shifted right $-n$ times if $n \le 0$.}
\Remarks{\name!~does not make a copy of $x$, which is therefore
modified after the call. It is unsafe to use the variable $x$
after the call, unless it has been assigned to the result
of the call, as in {\tt x := \name!(x, n)}.}
#endif
		default {
			commutative?:Boolean	== true;
			lcm(a:%, b:%):% 	== (a * b) quo gcd(a, b);
			next(a:%):%		== a + 1;
			prev(a:%):%		== a - 1;
			set(a:%, n:Z):%		== a \/ shift(1, n);
			clear(a:%, n:Z):%	== a /\ ~(shift(1, n));
			hash(a:%):Z		== machine a;
			odd?(a:%):Boolean	== ~even? a;
			shift!(a:%, n:Z):%	== shift(a, n);
			(a:%) mod (n:Z):Z	== machine(a mod (n::%));

			-- THE IMPLEMENTATION FROM Machine USES a % 2!!!
			even?(a:%):Boolean	== zero?(a /\ 1);

			factorial(n:%):% == {
				assert(n >= 0);
				m:% := 1; p := n;
				while p > 1 repeat {
					m := times!(m, p);
					p := prev p;
				}
				m;
			}

			(a:%) mod (b:%):% == {
				assert(b ~= 0);
				(r := a rem b) < 0 => r + abs b;
				r;
			}
		}
}
