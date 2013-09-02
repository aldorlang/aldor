------------------------------- sit_gexpcat.as ------------------------------
--
-- gec.as: A basic category for monomial domains looked as addtive monoids.
--
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
------------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{GeneralExponentCategory}
\History{Marc Moreno Maza}{11/7/2001}{created}
\Usage{\this: Category}
\Descr{\this~is the category of monomials looked as an additive
ordered monoid with a cancellation function and endowed with 
an order such that the addition is consistent with this order. 
By consistent addition we mean that $a \geq b$ implies $a + c \geq b + c$. 
By a cancellation function we mean an operation $f$ such that
$f(a,b)$ is either $failed$ or $c$ such that $a = b + c$ holds.
Here are two common examples of this operation.
For integers $f(a,b)$ is $a-b$ if $a \geq b$ and $failed$ otherwise.
For multivariate monomials $f(a,b)$ is $c$ if $a= bc$ and
$failed$ if no such $c$ exists.}
\begin{exports}
\category{\altype{ExpressionType}}\\
\category{\altype{TotallyOrderedType}}\\
\alfunc{AbelianMonoid}{$0$}: & \% & zero\\
\alfunc{AbelianMonoid}{$+$}: & (\%, \%) $\to$ \% & sum\\
\alfunc{AbelianMonoid}{add!}: & (\%, \%) $\to$ \% & in-place sum\\
\alexp{cancel}: & (\%, \%) $\to$ \% & cancellation\\
\alexp{cancel?}: & (\%, \%) $\to$ \altype{Boolean} & cancellation\\
\alexp{cancelIfCan}: & (\%, \%) $\to$ \altype{Partial} \% & cancellation\\
\alexp{times}: & (\altype{Integer}, \%) $\to$ \% & product by an integer\\
\alfunc{AbelianMonoid}{zero?}: & \% $\to$ \altype{Boolean} & test for $0$\\
\end{exports}
\Remarks{\this~is meant to implement efficient 
monomial arithmetic. In particular, for multivariate monomials
with a finite set of variables it is meant to code exponents
with primitive arrays of machine integers. Hence we cannot
claim that every exponent domain belongs to \altype{AbelianMonoid}.
Since we cannot subtract any exponent to every other,
we cannot claim that every exponent domain belongs to 
\altype{AdditiveType} neither, which
explains the need for this category.}
#endif

define GeneralExponentCategory: Category ==
	Join(TotallyOrderedType, ExpressionType) with {
        0: %;
        +: (%, %) -> %;
        add!: (%, %) -> %;
        zero?: % -> Boolean;
	cancel: (%, %) -> %; 
        cancel?: (%, %) -> Boolean;
	cancelIfCan: (%, %) -> Partial %;
#if ALDOC
\alpage{cancel,cancelIfCan}
\altarget{cancel}
\altarget{cancel?}
\altarget{cancelIfCan}
\Usage{cancel(x,y)\\ cancel?(x,y)\\ cancelIfCan(x,y)}
\Signatures{
cancel: & (\%, \%) $\to$ \%\\
cancel?: & (\%, \%) $\to$ \altype{Boolean}\\
cancelIfCan: & (\%, \%) $\to$ \altype{Partial} \%\\
}
\Params{ {\em x,y} & \% & Elements of the type \\ }
\Descr{cancelIfCan(x,y) returns {\em z} such that $z + y = x$ if
there exist such a {\em z} in the type viewed as a monoid only,
\failed~otherwise, while cancel?(x,y) returns whether cancelIfCan(x,y)
would fail, and cancel(x,y) returns {\em z} such that $z + y = x$, assuming that
cancelIfCan(x,y) would not fail.}
#endif
        times: (Integer, %) -> %;
#if ALDOC
\alpage{times}
\Usage{\name(n, x)}
\Signature{(\altype{Integer}, \%)}{\%}
\Params{
{\em n} & \altype{Integer} & An integer \\
{\em x} & \% & An element of the type \\
}
\Retval{Returns the product $n x$.}
#endif
}

extend MachineInteger: Join(Parsable, GeneralExponentCategory) == add {
	relativeSize(x:%):MachineInteger== abs x;
	cancel?(x:%, y:%):Boolean	== x >= y;
	cancel(x:%, y:%):%		== x - y;
	cancelIfCan(x:%, y:%):Partial %	== { x < y => failed; [x - y] }

	extree(x:%):ExpressionTree == {
		import from ExpressionTreeLeaf;
		extree leaf(x);
	}

	eval(t:ExpressionTreeLeaf):Partial % == {
		import from Integer;
		machineInteger? t => [machineInteger t];
		integer? t => [machine integer t];
		failed;
	}

	eval(op:MachineInteger, args:List ExpressionTree):Partial % == {
		import from ParsingTools %;
		evalArith(op, args);
	}

	times(n:Integer, x:%):% == {
		zero? x => x;
		machine(n) * x;
	}

}

