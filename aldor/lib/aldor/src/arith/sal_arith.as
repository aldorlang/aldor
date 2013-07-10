------------------------------ sal_arith.as ---------------------------------
--
-- Types with the most basic arithmetic operations
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{AdditiveType}
\History{Manuel Bronstein}{8/11/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types with addition/substraction operations.}
\begin{exports}
\category{\altype{PrimitiveType}}\\
\alexp{0} : & \% & zero\\
\alexp{+} : & (\%, \%) $\to$ \% & addition\\
\alexp{-} : & \% $\to$ \% & opposite\\
\alexp{-} : & (\%, \%) $\to$ \% & substraction\\
\alexp{add!}: & (\%, \%) $\to$ \% & In--place addition\\
\alexp{minus!}: & \% $\to$ \% & In--place opposite\\
\alexp{minus!}: & (\%, \%) $\to$ \% & In--place substraction\\
\alexp{zero?}: & \% $\to$ \altype{Boolean} & test for $0$\\
\end{exports}
#endif

define AdditiveType:Category == PrimitiveType with {
	0: %;
#if ALDOC
\alpage{0}
\Usage{\name}
\alconstant{\%}
\Retval{Return the $0$ constant of the type.}
#endif
	+: (%, %) -> %;
	-: % -> %;
	-: (%, %) -> %;
#if ALDOC
\alpage{+,-}
\altarget{+}
\altarget{-}
\Usage{$x + y$\\ $x - y$\\ $-x$}
\Signatures{
$-$: & \% $\to$ \%\\
$+,-$: & (\%, \%) $\to$ \%\\
}
\Params{ {\em x,y} & \% & elements of the type\\ }
\Retval{$x + y, x - y$ return respectively
the sum and difference $x$ with $y$, while $-x$ returns
the opposite of $x$.}
\alseealso{\alexp{add!}, \asexp{minus!}}
#endif
	add!: (%, %) -> %;
	minus!:% -> %;
	minus!:(%, %) -> %;
#if ALDOC
\alpage{add!,minus!}
\altarget{add!}
\altarget{minus!}
\Usage{add!(x, y)\\ minus!(x, y)\\ minus!~x}
\Signatures{
minus!: & \% $\to$ \%\\
add!, minus!: & (\%, \%) $\to$ \%\\
}
\Params{ {\em x, y} & \% & Elements of the type\\ }
\Retval{add!($x,y$) and minus!($x,y$) returns respectively $x + y$
and $x-y$, while minus!~x returns the opposite of $x$.
In all cases, the storage used by x is allowed
to be destroyed or reused, so x is lost after this call.}
\Remarks{Those functions may cause x to be destroyed, so do not use them
unless x has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
\alseealso{\alexp{+},\asexp{-}}
#endif
	zero?: % -> Boolean;
#if ALDOC
\alpage{zero?}
\Usage{\name~x}
\Signature{\%}{\altype{Boolean}}
\Params{{\em x} & \% & an element of the type\\ }
\Retval{Returns the result of $x = 0$ using the semantics of $=$ of the type.}
#endif
	default {
		local copy?:Boolean	== % has CopyableType;
		(a:%) - (b:%):%		== a + (-b);
		zero?(a:%):Boolean	== a = 0;
		minus!(a:%):%		== -a;
		minus!(a:%, b:%):%	== a - b;

		add!(a:%, b:%):% == {
			zero? a => { copy?=>copy(b)$(% pretend CopyableType);b }
			a + b;
		}
	}
}

#if ALDOC
\thistype{ArithmeticType}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types with standard arithmetic operations.}
\begin{exports}
\category{\altype{AdditiveType}}\\
\alexp{1}: & \% & one\\
\alexp{*}: & (\%, \%) $\to$ \% & product\\
\alalias{\this}{**}{$\land$}:
& (\%, \altype{MachineInteger}) $\to$ \% & exponentiation\\
\alexp{commutative?}: & \altype{Boolean} & check whether $*$ is commutative\\
\alexp{one?}: & \% $\to$ \altype{Boolean} & test for $1$\\
\alexp{times!}: & (\%, \%) $\to$ \% & In--place product\\
\end{exports}
#endif

define ArithmeticType:Category == AdditiveType with {
	1: %;
#if ALDOC
\alpage{1}
\Usage{\name}
\alconstant{\%}
\Retval{Return the $1$ constant of the type.}
#endif
	*: (%, %) -> %;
	^: (%, MachineInteger) -> %;
#if ALDOC
\alpage{*,**}
\altarget{*}
\altarget{**}
\Usage{$x \ast y$\\ $x\land n$}
\Signatures{
$\ast$: & (\%, \%) $\to$ \%\\
$\land$: & (\%, \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em x,y} & \% & elements of the type\\
{\em n} & \altype{MachineInteger} & an exponent\\
}
\Retval{$x \ast y$ returns the product of $x$ with $y$, while
$x\land n$ returns $x$ to the power $n$.}
\alseealso{\alexp{times!}}
#endif
	commutative?: Boolean;
#if ALDOC
\alpage{commutative?}
\Usage{\name}
\alconstant{\altype{Boolean}}
\Retval{Returns \true~if \alexp{*} is commutative,
\false~otherwise.}
#endif
	one?: % -> Boolean;
#if ALDOC
\alpage{one?}
\Usage{\name~x}
\Signature{\%}{\altype{Boolean}}
\Params{{\em x} & \% & an element of the type\\ }
\Retval{Returns the result of $x = 1$ using the semantics of $=$ of the type.}
#endif
	times!:(%, %) -> %;
#if ALDOC
\alpage{times!}
\Usage{\name(x, y)}
\Signature{(\%, \%)}{\%}
\Params{ {\em x, y} & \% & Elements of the type\\ }
\Retval{Return $xy$, where the storage used by x is allowed
to be destroyed or reused, so x is lost after this call.}
\Remarks{This function may cause x to be destroyed, so do not use it unless
x has been locally allocated, and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
\alseealso{\alexp{*}}
#endif
	default {
		local copy?:Boolean	== % has CopyableType;
		one?(a:%):Boolean	== a = 1;

		times!(a:%, b:%):% == {
			one? a => { copy?=>copy(b)$(% pretend CopyableType);b }
			a * b;
		}
	}
}

